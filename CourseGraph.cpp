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

    // === Prerequisites (examples) ===
    // Math
    addEdge("MATH132401E - Calculus 1", "MATH132501E - Calculus 2");
    addEdge("MATH132501E - Calculus 2", "MATH143001E - Linear Algebra and Algebraic Structure");

    // Programming Core
    addEdge("INPR130285E - Introduction to Programming", "PRTE230385E - Programming Techniques");
    addEdge("PRTE230385E - Programming Techniques", "DASA230179E - Data Structure and Algorithm");
    addEdge("DASA230179E - Data Structure and Algorithm", "OOPR230279E - Object-Oriented Programming");

    // Database
    addEdge("OOPR230279E - Object-Oriented Programming", "DBSY230184E - Database Systems");

    // Networking
    addEdge("INTE230187E - Computer Networks", "PRNE230282E - Computer Networking Practice");

    // Software Engineering
    addEdge("OOPR230279E - Object-Oriented Programming", "SOEN230289E - Software Engineering");
    addEdge("SOEN230289E - Software Engineering", "SYDE230281E - Systems Analysis and Design");

    // AI / ML
    addEdge("PRCO230383E - Probability & Statistics", "MALE230291E - Machine Learning");
    addEdge("MALE230291E - Machine Learning", "NEDE230298E - Neural Networks & Deep Learning");

    // Capstone & Internship
    addEdge("SYDE230281E - Systems Analysis and Design", "CAPS330399E - Capstone Project 1");
    addEdge("CAPS330399E - Capstone Project 1", "CAPS430499E - Capstone Project 2");
    addEdge("CAPS430499E - Capstone Project 2", "INTE430497E - Internship");
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


QVariantList CourseGraph::generateStudyPlan(int completedSemesters,
                                            int targetSemesters,
                                            const QList<QString> &completedCourses)
{
    QVariantList result;

    // 1. Topological sort
    QStringList order = topoSort();
    if (order.isEmpty()) {
        emit errorOccurred("Topological sort failed (maybe cycle in graph).");
        return result;
    }

    // 2. Bỏ các môn trong danh sách completedCourses (sinh viên tick đã học)
    for (const QString &done : completedCourses) {
        order.removeAll(done);
    }

    // 3. Tính số kỳ còn lại
    int remainSemesters = targetSemesters - completedSemesters;
    if (remainSemesters <= 0) {
        emit errorOccurred("Target semesters must be greater than completed semesters.");
        return result;
    }

    // 4. Chia đều danh sách còn lại
    int perSemester = qCeil((double)order.size() / remainSemesters);
    int idx = 0;
    for (int s = completedSemesters + 1; s <= targetSemesters; ++s) {
        QStringList semCourses;
        for (int j = 0; j < perSemester && idx < order.size(); ++j, ++idx) {
            semCourses << order[idx];
        }

        if (!semCourses.isEmpty()) {
            // Đưa vào QVariantMap để QML đọc được
            QVariantMap sem;
            sem["semester"] = s;
            sem["courses"] = semCourses;
            result << sem;
        }
    }

    return result;
}



QString CourseGraph::dotFromCourses(const QStringList &courses) const {
    QString dot = "digraph G { rankdir=LR; node [shape=round];\n";
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
