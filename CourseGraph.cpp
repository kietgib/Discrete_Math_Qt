#include "coursegraph.h"
#include <QProcess>
#include <QBuffer>
#include <QDebug>

CourseGraph::CourseGraph(QObject *parent) : QObject(parent) {
    buildSampleGraph();
}

void CourseGraph::buildSampleGraph() {
    m_courses = {
        // Semester 1
        "Philosophy of Marxism-Leninism",
        "Calculus 1",
        "Academic English 1",
        "Academic English 2",
        "Introduction to IT",
        "Introduction to Programming",
        "Physical Education 1",

        // Semester 2
        "Political Economics of Marxism-Leninism",
        "Calculus 2",
        "Linear Algebra and Algebraic Structure",
        "Academic English 3",
        "Academic English 4",
        "Programming Techniques",
        "Physics 1",
        "Physical Education 2",

        // Semester 3
        "Scientific Socialism",
        "Discrete Mathematics & Graphs Theory",
        "Data Structure and Algorithm",
        "Object-Oriented Programming",
        "Basic Electronics (IT)",
        "Database Systems",
        "Physics 1 Lab",
        "Physical Education 3",

        // Semester 4
        "Ho Chi Minh Ideology",
        "Operating Systems",
        "Probability & Statistics",
        "Software Engineering",
        "Computer Architecture",
        "Computer Networks",
        "Physical Education 4",

        // Semester 5
        "History of Vietnamese Communist Party",
        "Systems Analysis and Design",
        "Computer Networking Practice",
        "Web Programming",
        "Principles of Data Analytics",
        "Mobile Programming",
        "Principles of Information Security",

        // Semester 6
        "Information Retrieval",
        "Machine Learning",
        "Image Processing",
        "Distributed Systems",
        "Human-Computer Interaction",
        "Cloud Computing",

        // Semester 7
        "Network Security",
        "Programming Paradigms",
        "Neural Networks & Deep Learning",
        "Capstone Project 1",

        // Semester 8
        "Capstone Project 2",
        "Internship"
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

    m_adj.resize(m_courses.size());

    // Helper: add edge by course names
    auto addEdge = [&](const QString &u, const QString &v){
        int iu = m_courses.indexOf(u);
        int iv = m_courses.indexOf(v);
        if (iu >= 0 && iv >= 0) {
            m_adj[iu].append(iv);
        } else {
            qWarning() << "Invalid course edge:" << u << "->" << v;
        }
    };

    // Prerequisites
    // Math
    addEdge("Calculus 1", "Calculus 2");
    addEdge("Calculus 2", "Discrete Mathematics & Graphs Theory");
    addEdge("Calculus 2", "Linear Algebra and Algebraic Structure");

    // Politics
    addEdge("Philosophy of Marxism-Leninism","Political Economics of Marxism-Leninism");
    addEdge("Philosophy of Marxism-Leninism","Scientific Socialism");
    addEdge("Philosophy of Marxism-Leninism","Ho Chi Minh Ideology");
    addEdge("Political Economics of Marxism-Leninism","History of Vietnamese Communist Party");

    // Academic English
    addEdge("Academic English 1", "Academic English 3");
    addEdge("Academic English 2","Academic English 4");

    // Physics
    addEdge("Physical Education 1","Physical Education 2");
    addEdge("Physical Education 2","Physical Education 3");
    addEdge("Physical Education 1","Physics 1 Lab");
    addEdge("Physical Education 3","Physical Education 4");

    // Programming Core
    addEdge("Introduction to Programming", "Programming Techniques");
    addEdge("Programming Techniques", "Data Structure and Algorithm");
    addEdge("Data Structure and Algorithm", "Object-Oriented Programming");

    // Database
    addEdge("Object-Oriented Programming", "Database Systems");

    // Networking
    addEdge("Computer Networks", "Computer Networking Practice");

    // Software Engineering
    addEdge("Object-Oriented Programming", "Software Engineering");
    addEdge("Software Engineering", "Systems Analysis and Design");

    // AI / ML
    addEdge("Probability & Statistics", "Machine Learning");
    addEdge("Machine Learning", "Neural Networks & Deep Learning");

    // Capstone & Internship
    addEdge("Systems Analysis and Design", "Capstone Project 1");
    addEdge("Capstone Project 1", "Capstone Project 2");
    addEdge("Capstone Project 2", "Internship");
}

QStringList CourseGraph::topoSort() {
    QStringList result;

    int n = m_courses.size();
    QVector<int> indeg(n, 0);

    // 1. Tính indegree
    for (int u = 0; u < n; ++u) {
        for (int v : m_adj[u]) {
            indeg[v]++;
        }
    }

    // 2. Queue cho các node indegree = 0
    QList<int> q;
    for (int i = 0; i < n; ++i) {
        if (indeg[i] == 0)
            q.append(i);
    }

    // 3. Kahn’s algorithm
    while (!q.isEmpty()) {
        int u = q.takeFirst();
        result << m_courses[u];

        for (int v : m_adj[u]) {
            indeg[v]--;
            if (indeg[v] == 0)
                q.append(v);
        }
    }

    // 4. Kiểm tra có chu trình không
    if (result.size() != n) {
        emit errorOccurred("Graph has a cycle! Cannot do topological sort.");
        return {};
    }

    return result;
}
QVariantList CourseGraph::searchCourse(const QString &keyword) {
    QVariantList result;
    for (int i = 0; i < m_courses.size(); i++) {
        if (m_courses[i].contains(keyword, Qt::CaseInsensitive)) {
            QVariantMap item;
            item["name"] = m_courses[i];
            item["semester"] = m_semesters[i];
            result << item;
        }
    }
    return result;
}
QVariantList CourseGraph::generateStudyPlan(int completedSemesters,
                                            int targetSemesters,
                                            const QStringList &completedCourses)
{
    QVariantList result;

    // 1. Topological sort toàn bộ
    QStringList order = topoSort();
    if (order.isEmpty()) {
        emit errorOccurred("Topological sort failed (maybe cycle in graph).");
        return result;
    }

    // 2. Xóa tất cả môn thuộc các kỳ đã hoàn thành
    for (int i = 0; i < m_courses.size(); i++) {
        if (m_semesters[i] <= completedSemesters) {
            order.removeAll(m_courses[i]);
        }
    }

    // 3. Thêm lại các môn đã tick (nếu còn trong graph)
    QStringList mergedCourses = order;
    for (const QString &c : completedCourses) {
        if (!mergedCourses.contains(c) && m_courses.contains(c)) {
            mergedCourses << c;
        }
    }

    // 4. Chạy lại topoSort nhưng chỉ giữ các môn trong mergedCourses
    QStringList sorted;
    QSet<QString> keepSet = QSet<QString>(mergedCourses.begin(), mergedCourses.end());

    // copy lại graph để lọc
    int n = m_courses.size();
    QVector<int> indeg(n, 0);
    for (int u = 0; u < n; ++u) {
        for (int v : m_adj[u]) {
            if (keepSet.contains(m_courses[u]) && keepSet.contains(m_courses[v]))
                indeg[v]++;
        }
    }

    QList<int> q;
    for (int i = 0; i < n; ++i) {
        if (keepSet.contains(m_courses[i]) && indeg[i] == 0)
            q.append(i);
    }

    while (!q.isEmpty()) {
        int u = q.takeFirst();
        if (!keepSet.contains(m_courses[u])) continue;
        sorted << m_courses[u];
        for (int v : m_adj[u]) {
            if (keepSet.contains(m_courses[v])) {
                indeg[v]--;
                if (indeg[v] == 0)
                    q.append(v);
            }
        }
    }
    // 5. Chia đều sorted vào các kỳ còn lại
    int remainSemesters = targetSemesters - completedSemesters;
    int total = sorted.size();
    int base = total / remainSemesters;   // số môn tối thiểu mỗi kỳ
    int extra = total % remainSemesters;  // số kỳ đầu được +1 môn
    int idx = 0;
    for (int i = 0; i < remainSemesters; ++i) {
        int semNo = completedSemesters + i + 1;
        int count = base + (i < extra ? 1 : 0);  // chia đều và dàn phần dư
        QStringList semCourses;

        for (int j = 0; j < count && idx < total; ++j, ++idx) {
            semCourses << sorted[idx];
        }

        QVariantMap sem;
        sem["semester"] = semNo;
        sem["courses"] = semCourses;
        result << sem;
    }


    return result;
}


// Xu li do thi
QString CourseGraph::dotFromCourses(const QStringList &courses) const {
    QString dot = "digraph prereq {\n"
                  "rankdir=LR;\n"
                  "node [shape=box, style=rounded, fontsize=10, margin=\"0.6,0.4\", width=0, height=0, fixedsize=false];\n"
                  "edge [arrowsize=0.7];\n";
    for (int u = 0; u < m_courses.size(); ++u) {
        if (!courses.contains(m_courses[u])) continue;
        dot += "\"" + m_courses[u] + "\";\n";
        for (int v : m_adj[u]) {
            if (courses.contains(m_courses[v]))
                dot += "\"" + m_courses[u] + "\" -> \"" + m_courses[v] + "\";\n";
        }
    }
    dot += "}\n";
    return dot;
}

QString CourseGraph::svgBase64FromDot(const QString &dot) const {
    QProcess process;
    process.start("dot", QStringList() << "-Tsvg");
    process.write(dot.toUtf8());
    process.closeWriteChannel();
    process.waitForFinished();
    QByteArray svgData = process.readAllStandardOutput();
    if (svgData.isEmpty()) return "";
    return "data:image/svg+xml;base64," + svgData.toBase64();
}

QString CourseGraph::svgForPlan(const QStringList &courses) {
    QString dot = dotFromCourses(courses);
    return svgBase64FromDot(dot);
}
