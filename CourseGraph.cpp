#include "CourseGraph.h"
#include <QProcess>
#include <QDebug>
#include <algorithm>
#include <QLine>


CourseGraph::CourseGraph(QObject *parent) : QObject(parent) {
    buildSampleGraph();
}

void CourseGraph::buildSampleGraph() {
    m_courses = {
        // Semester 1
        "LLCT130105E - Philosophy of Marxism-Leninism",
        "MATH132401E - Calculus 1",
        "ACEN340535E - Academic English 1",
        "ACEN340635E - Academic English 2",
        "INIT130185E - Introduction to IT",
        "INPR130285E - Introduction to Programming",
        "PHED110130 - Physical Education 1",

        // Semester 2
        "LLCT120205E - Political Economics of Marxism-Leninism",
        "MATH132501E - Calculus 2",
        "MATH143001E - Linear Algebra and Algebraic Structure",
        "ACEN440735E - Academic English 3",
        "ACEN440835E - Academic English 4",
        "PRTE230385E - Programming Techniques",
        "PHYS130902E - Physics 1",
        "PHED110230 - Physical Education 2",

        // Semester 3
        "LLCT120405E - Scientific Socialism",
        "DIGR230485E - Discrete Mathematics & Graphs Theory",
        "DASA230179E - Data Structure and Algorithm",
        "OOPR230279E - Object-Oriented Programming",
        "EEEN231780E - Basic Electronics (IT)",
        "DBSY230184E - Database Systems",
        "PHYS111202E - Physics 1 Lab",
        "PHED110330 - Physical Education 3",

        // Semester 4
        "LLCT120505E - Ho Chi Minh Ideology",
        "OPSY230186E - Operating Systems",
        "PRCO230383E - Probability & Statistics",
        "SOEN230289E - Software Engineering",
        "ARCH230189E - Computer Architecture",
        "INTE230187E - Computer Networks",
        "PHED110430 - Physical Education 4",

        // Semester 5
        "LLCT120605E - History of Vietnamese Communist Party",
        "SYDE230281E - Systems Analysis and Design",
        "PRNE230282E - Computer Networking Practice",
        "WEBP230286E - Web Programming",
        "PRDA230284E - Principles of Data Analytics",
        "MOPR230287E - Mobile Programming",
        "PRSY230288E - Principles of Information Security",

        // Semester 6
        "INTR230290E - Information Retrieval",
        "MALE230291E - Machine Learning",
        "IMPR230292E - Image Processing",
        "DIST230293E - Distributed Systems",
        "HUPR230294E - Human-Computer Interaction",
        "CLOD230295E - Cloud Computing",

        // Semester 7
        "NEST230296E - Network Security",
        "PRPR230297E - Programming Paradigms",
        "NEDE230298E - Neural Networks & Deep Learning",
        "CAPS330399E - Capstone Project 1",

        // Semester 8
        "CAPS430499E - Capstone Project 2",
        "INTE430497E - Internship"
    };

    m_semesters = {
        // Sem 1
        1,1,1,1,1,1,1,
        // Sem 2
        2,2,2,2,2,2,2,2,
        // Sem 3
        3,3,3,3,3,3,3,3,
        // Sem 4
        4,4,4,4,4,4,4,
        // Sem 5
        5,5,5,5,5,5,5,
        // Sem 6
        6,6,6,6,6,6,
        // Sem 7
        7,7,7,7,
        // Sem 8
        8,8
    };
    int n = m_courses.size();
    m_adj.assign(n, QVector<int>());

    auto addEdge = [&](int u, int v){
        if (u>=0 && u<n && v>=0 && v<n)
            m_adj[u].append(v);
    };

    // Toán
    addEdge(0, 5);   // MA101 -> MA102
    addEdge(0, 10);  // MA101 -> MA201
    addEdge(10, 15); // MA201 -> MA202

    // CNTT cơ sở
    addEdge(2, 7);   // IT101 -> IT102
    addEdge(7, 11);  // IT102 -> IT201
    addEdge(11, 17); // IT201 -> IT203
    addEdge(17, 22); // IT203 -> IT303
    addEdge(11, 18); // IT201 -> IT204
    addEdge(12, 20); // IT202 -> IT301

    // HTTT & Mạng
    addEdge(18, 21); // IT204 -> IT302
    addEdge(19, 25); // IT205 -> IT305

    // AI, ML, DL
    addEdge(26, 30); // IT306 -> IT401
    addEdge(27, 31); // IT308 -> IT402
    addEdge(21, 32); // IT302 -> IT403
    addEdge(20, 33); // IT301 -> IT404
    addEdge(30, 34); // IT401 -> IT405

    // Đồ án & Thực tập (kỳ 8)
    addEdge(32, 35); // IT403 -> IT406
    addEdge(34, 35); // IT405 -> IT406
    addEdge(35, 36); // IT406 -> IT407 (có thể yêu cầu xong đồ án mới đi thực tập, tuỳ chương trình)


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
    QSet<int> s(m_semesters.begin(), m_semesters.end());
    QList<int> list = s.values();
    std::sort(list.begin(), list.end());

    QVariantList out;
    for (int v : list) {
        out << v;
    }
    return out;
}

QStringList CourseGraph::courses() const {
    return QStringList(m_courses.begin(), m_courses.end());
}


QString CourseGraph::dotFromGraph() const {
    QString dot = "digraph prereq {\n"
                  "rankdir=LR;\n"
                  "node [shape=box, style=rounded, fontsize=10, margin=\"0.6,0.4\", width=0, height=0, fixedsize=false];\n"
                  "edge [arrowsize=0.7];\n";

    for (int i = 0; i < m_courses.size(); ++i) {
        dot += QString("\"%1 \";\n").arg(m_courses[i].toHtmlEscaped());
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
        if (vis[nb] == 1) return false;
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
    QProcess proc;
    proc.start("dot", {"-Tsvg"});
    if (!proc.waitForStarted(3000)) {
        emit errorOccurred("Graphviz 'dot' not found.");
        return QString{};
    }
    proc.write(m_dot.toUtf8());
    proc.closeWriteChannel();
    proc.waitForFinished(5000);

    QByteArray svg = proc.readAllStandardOutput();
    if (svg.isEmpty()) {
        emit errorOccurred("Graphviz returned no SVG.");
        return QString{};
    }

    return "data:image/svg+xml;base64," + svg.toBase64();
}



Q_INVOKABLE void CourseGraph::setCourseSelected(const QString &course, bool selected) {
    if (selected) {
        if (!selectedCourses.contains(course))
            selectedCourses.append(course);
    } else {
        selectedCourses.removeAll(course);
    }
}

QString CourseGraph::svgForCourse(const QString &courseName) {
    int idx = m_courses.indexOf(courseName);
    if (idx < 0) {
        emit errorOccurred("Course not found: " + courseName);
        return QString{};
    }

    QSet<int> related;
    related.insert(idx);

    // prerequisite
    for (int u = 0; u < m_adj.size(); ++u) {
        for (int v : m_adj[u]) {
            if (v == idx) related.insert(u);
        }
    }
    // dependent
    for (int v : m_adj[idx]) related.insert(v);

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
QString CourseGraph::svgForCourses(const QStringList &courseNames) {
    QSet<int> related;

    // tìm index các môn đã chọn
    for (const QString &c : courseNames) {
        int idx = m_courses.indexOf(c);
        if (idx >= 0) related.insert(idx);
    }

    if (related.isEmpty()) {
        emit errorOccurred("No courses selected.");
        return QString{};
    }

    // thêm prerequisite và dependent của các môn chọn
    QList<int> relList = related.values();
    for (int k = 0; k < relList.size(); ++k) {
        int idx = relList[k];
        // prerequisite
        for (int u = 0; u < m_adj.size(); ++u) {
            for (int v : m_adj[u]) {
                if (v == idx) related.insert(u);
            }
        }
        // dependent
        for (int v : m_adj[idx]) related.insert(v);
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
