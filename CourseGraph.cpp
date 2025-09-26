#include "CourseGraph.h"
#include <QProcess>
#include <QDebug>

CourseGraph::CourseGraph(QObject *parent) : QObject(parent) {
    buildSampleGraph();
}

void CourseGraph::buildSampleGraph() {
    m_courses = {
        "MA1101 - Mathematics IA",        // 0
        "IF2120 - Discrete Mathematics",  // 1
        "MA1201 - Mathematics IIA",       // 2
        "IF2110 - Algorithm & Data Str",  // 3
        "IF2121 - Computational Logic",   // 4
        "IF2124 - Formal Lang & Automata",// 5
        "IF2220 - Probability & Statistics",//6
        "IF2211 - Algorithm Strategies",  // 7
        "IF3170 - Artificial Intelligence",//8
        "IF2130 - Computer Org & Arch",   //9
        "IF2123 - Geometric & Linear Alg",//10
        "IF3270 - Machine Learning",      //11
        "IF3260 - Computer Graphics",     //12
        "IF4073 - Image Interpretation & Processing" //13
    };

    m_semesters = {
        1,1,2,2,2,3,3,3,1,2,3,1,2,3
    };

    int n = m_courses.size();
    m_adj.assign(n, QVector<int>());

    auto addEdge = [&](int u, int v){
        if (u>=0 && u<n && v>=0 && v<n)
            m_adj[u].append(v);
    };

    addEdge(0,6);
    addEdge(0,10);
    addEdge(6,8);
    addEdge(8,11);
    addEdge(11,13);
    addEdge(10,12);
    addEdge(12,13);
    addEdge(3,7);
    addEdge(7,9);
    addEdge(2,3);
    addEdge(1,5);
    addEdge(1,3);
    addEdge(4,3);

    m_dot = dotFromGraph();
    emit coursesChanged();
}

QStringList CourseGraph::coursesForSemester(int semester) const {
    QStringList result;
    for (int i = 0; i < m_courses.size(); ++i) {
        if (i < m_semesters.size() && m_semesters[i] == semester) {
            result << m_courses[i];
        }
    }
    return result;
}

QVariantList CourseGraph::availableSemesters() const {
    QSet<int> s;
    for (int sem : m_semesters) {
        s.insert(sem);
    }
    QList<int> list = s.values();
    std::sort(list.begin(), list.end());

    QVariantList out;
    for (int v : list) {
        out.append(v);
    }
    return out;
}

QString CourseGraph::dotFromGraph() const {
    QString dot = "digraph prereq {\n"
                  "rankdir=LR;\n"  // vẽ ngang thay vì dọc
                  "node [shape=box, style=rounded, fontsize=10, margin=0.1];\n"
                  "edge [arrowsize=0.7];\n";

    for (int i = 0; i < m_courses.size(); ++i) {
        dot += QString("\"%1\";\n").arg(m_courses[i].toHtmlEscaped());
    }

    for (int u = 0; u < m_adj.size(); ++u) {
        for (int v : m_adj[u]) {
            dot += QString("\"%1\" -> \"%2\";\n")
            .arg(m_courses[u].toHtmlEscaped())
                .arg(m_courses[v].toHtmlEscaped());
        }
    }

    dot += "}\n";
    return dot;
}


bool CourseGraph::dfsTopoUtil(int v, QVector<int> &vis, QStringList &stack) const {
    vis[v] = 1;
    for (int nb : m_adj[v]) {
        if (vis[nb] == 1) {
            return false;
        }
        if (vis[nb] == 0) {
            if (!dfsTopoUtil(nb, vis, stack)) return false;
        }
    }
    vis[v] = 2;
    stack.prepend(m_courses[v]);
    return true;
}

QStringList CourseGraph::topoSort() {
    int n = m_courses.size();
    QVector<int> vis(n, 0);
    QStringList result;
    for (int i = 0; i < n; ++i) {
        if (vis[i] == 0) {
            if (!dfsTopoUtil(i, vis, result)) {
                emit errorOccurred("Graph has a cycle!");
                return QStringList{};
            }
        }
    }
    return result;
}

QString CourseGraph::svgBase64FromDot() {
    QString dot = m_dot;
    if (dot.isEmpty()) {
        emit errorOccurred("DOT is empty");
        return QString{};
    }

    QProcess proc;
    proc.start("dot", {"-Tsvg"});
    if (!proc.waitForStarted(3000)) {
        emit errorOccurred("Graphviz 'dot' not found.");
        return QString{};
    }
    proc.write(dot.toUtf8());
    proc.closeWriteChannel();
    proc.waitForFinished(5000);

    QByteArray svg = proc.readAllStandardOutput();
    if (svg.isEmpty()) {
        emit errorOccurred("Graphviz returned no SVG.");
        return QString{};
    }

    return "data:image/svg+xml;base64," + svg.toBase64();
}

QString CourseGraph::svgForCourse(const QString &courseName) {
    int idx = m_courses.indexOf(courseName);
    if (idx < 0) {
        emit errorOccurred("Không tìm thấy môn học: " + courseName);
        return QString{};
    }

    QSet<int> related;
    related.insert(idx);

    for (int u = 0; u < m_adj.size(); ++u) {
        for (int v : m_adj[u]) {
            if (v == idx) related.insert(u);
        }
    }
    for (int v : m_adj[idx]) {
        related.insert(v);
    }

    QString dot = "digraph prereq {\n\trankdir=LR;\n\tnode [shape=box, style=rounded];\n";
    for (int i : related) {
        dot += QString("\tN%1 [label=\"%2\"];\n").arg(i).arg(m_courses[i].toHtmlEscaped());
    }
    for (int u = 0; u < m_adj.size(); ++u) {
        for (int v : m_adj[u]) {
            if (related.contains(u) && related.contains(v)) {
                dot += QString("\tN%1 -> N%2;\n").arg(u).arg(v);
            }
        }
    }
    dot += "}\n";

    QProcess proc;
    proc.start("dot", {"-Tsvg"});
    if (!proc.waitForStarted(3000)) {
        emit errorOccurred("Không chạy được dot (Graphviz).");
        return QString{};
    }
    proc.write(dot.toUtf8());
    proc.closeWriteChannel();
    proc.waitForFinished(5000);

    QByteArray svg = proc.readAllStandardOutput();
    if (svg.isEmpty()) {
        emit errorOccurred("Không sinh được SVG từ Graphviz.");
        return QString{};
    }

    return "data:image/svg+xml;base64," + svg.toBase64();
}
