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
        "Physics 1",
        // Semester 2
        "Political Economics of Marxism-Leninism",
        "Calculus 2",
        "Linear Algebra and Algebraic Structure",
        "Academic English 3",
        "Academic English 4",
        "Programming Techniques",
        // Semester 3
        "Scientific Socialism",
        "Discrete Mathematics & Graphs Theory",
        "Data Structure and Algorithm",
        "Object-Oriented Programming",
        "Basic Electronics (IT)",
        "Database Systems",
        "Physics 1 Lab",
        // Semester 4
        "Ho Chi Minh Ideology",
        "Operating Systems",
        "Probability & Statistics",
        "Software Engineering",
        "Computer Architecture",
        "Computer Networks",
        // Semester 5
        "History of Vietnamese Communist Party",
        "Systems Analysis and Design",
        "Computer Networking Practice",
        "Web Programming",
        "Principles of Data Analytics",
        "Mobile Programming",
        "Principles of Information Security",
        // Ky 6
        "Information Retrieval",
        "Machine Learning",
        "Image Processing",
        "Distributed Systems",
        "Human-Computer Interaction",
        "Cloud Computing",
        // Ky 7
        "Network Security",
        "Programming Paradigms",
        "Neural Networks & Deep Learning",
        "Capstone Project 1",
        // Ky 8
        "Capstone Project 2",
        "Internship"
    };

    m_semesters = {
        // Sem 1
        1,1,1,1,1,1,
        // Sem 2
        2,2,2,2,2,2,2,
        // Sem 3
        3,3,3,3,3,3,3,
        // Sem 4
        4,4,4,4,4,4,
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

    // Lien ket
    // Math
    addEdge("Calculus 1", "Calculus 2");
    addEdge("Calculus 2", "Discrete Mathematics & Graphs Theory");
    addEdge("Calculus 2", "Linear Algebra and Algebraic Structure");
    addEdge("Discrete Mathematics & Graphs Theory", "Data Structure and Algorithm");

    // Politics
    addEdge("Philosophy of Marxism-Leninism","Political Economics of Marxism-Leninism");
    addEdge("Philosophy of Marxism-Leninism","Scientific Socialism");
    addEdge("Philosophy of Marxism-Leninism","Ho Chi Minh Ideology");
    addEdge("Political Economics of Marxism-Leninism","History of Vietnamese Communist Party");

    // English
    addEdge("Academic English 1", "Academic English 2");
    addEdge("Academic English 2", "Academic English 3");
    addEdge("Academic English 3", "Academic English 4");

    // Physics
    addEdge("Physics 1","Basic Electronics (IT)");
    addEdge("Physics 1","Physics 1 Lab");

    // Programming Core
    addEdge("Introduction to IT", "Programming Techniques");
    addEdge("Introduction to Programming", "Programming Techniques");
    addEdge("Programming Techniques", "Data Structure and Algorithm");
    addEdge("Data Structure and Algorithm", "Operating Systems");
    addEdge("Data Structure and Algorithm", "Probability & Statistics");
    addEdge("Programming Techniques", "Object-Oriented Programming");
    addEdge("Data Structure and Algorithm", "Software Engineering");
    addEdge("Database Systems", "Software Engineering");
    addEdge("Linear Algebra and Algebraic Structure","Database Systems");

    // Database & Software Eng
    addEdge("Object-Oriented Programming", "Database Systems");
    addEdge("Object-Oriented Programming", "Software Engineering");
    addEdge("Software Engineering", "Systems Analysis and Design");

    // Networking
    addEdge("Database Systems","Computer Architecture");
    addEdge("Computer Architecture","Computer Networks");
    addEdge("Computer Networks", "Computer Networking Practice");

    // AI / ML
    addEdge("Probability & Statistics", "Machine Learning");
    addEdge("Machine Learning", "Neural Networks & Deep Learning");

    // Advanced Systems
    addEdge("Distributed Systems","Cloud Computing");
    addEdge("Cloud Computing","Network Security");
    addEdge("Network Security","Programming Paradigms");

    // Capstone & Internship
    addEdge("Systems Analysis and Design", "Capstone Project 1");
    addEdge("Capstone Project 1", "Capstone Project 2");
    addEdge("Capstone Project 2", "Internship");

    // Web & App Dev
    addEdge("Database Systems", "Web Programming");
    addEdge("Web Programming", "Mobile Programming");
    addEdge("Web Programming", "Principles of Data Analytics");
    addEdge("Web Programming", "Principles of Information Security");

    // Advanced Systems
    addEdge("Operating Systems", "Distributed Systems");
    addEdge("Distributed Systems", "Human-Computer Interaction");
    addEdge("Distributed Systems", "Information Retrieval");
    addEdge("Distributed Systems", "Image Processing");

    // Security & AI branch
    addEdge("Principles of Information Security", "Network Security");

    // ML support
    addEdge("Data Structure and Algorithm", "Information Retrieval");
    addEdge("Machine Learning", "Image Processing");
}

QStringList CourseGraph::topoSort() {
    QStringList result;

    int n = m_courses.size();
    QVector<int> indeg(n, 0);

    // 1. Tính bac cua node
    for (int u = 0; u < n; ++u) {
        for (int v : m_adj[u]) {
            indeg[v]++;
        }
    }

    // 2. Queue cho các node voi bac=0
    QList<int> q;
    for (int i = 0; i < n; ++i) {
        if (indeg[i] == 0)
            q.append(i);
    }

    // 3. Bfs
    while (!q.isEmpty()) {
        int u = q.takeFirst();
        result << m_courses[u];

        for (int v : m_adj[u]) {
            indeg[v]--;
            if (indeg[v] == 0)
                q.append(v);
        }
    }

    // 4. Check test
    if (result.size() != n) {
        emit errorOccurred("Cannot sorting");
        return {};
    }

    return result;
}
QVariantList CourseGraph::searchCourse(const QString &keyword) {    // Dung trong phan tim kiem cua UI
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
QVariantList CourseGraph::generateStudyPlan(int completedSemesters,int targetSemesters,const QStringList &completedCourses)
{
    QVariantList result;

    // 1. Topologicalsort
    QStringList order = topoSort();
    if (order.isEmpty()) {
        emit errorOccurred("Cannot sorting");
        return result;
    }

    // 2. Xoa cac mon thuoc ki chon da hoan thanh
    for (int i = 0; i < m_courses.size(); i++) {
        if (m_semesters[i] <= completedSemesters) {
            order.removeAll(m_courses[i]);
        }
    }

    // 3. Them lai mon vao list
    QStringList mergedCourses = order;
    for (const QString &c : completedCourses) {
        if (!mergedCourses.contains(c) && m_courses.contains(c)) {
            mergedCourses << c;
        }
    }

    // 4. Topo again
    QStringList sorted;
    QSet<QString> keepSet = QSet<QString>(mergedCourses.begin(), mergedCourses.end());

    // loc graph
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

    // 5. Chia list theo layer
    QVector<QStringList> layers;
    {
        // tính indegree cho tung node
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
            QStringList layer;
            int layerSize = q.size();
            for (int k = 0; k < layerSize; ++k) {
                int u = q.takeFirst();
                if (!keepSet.contains(m_courses[u])) continue;
                layer << m_courses[u];
                for (int v : m_adj[u]) {
                    if (keepSet.contains(m_courses[v])) {
                        indeg[v]--;
                        if (indeg[v] == 0)
                            q.append(v);
                    }
                }
            }
            if (!layer.isEmpty()) layers.append(layer);
        }
    }
    // chia lại các layer thành semester
    int remainSemesters = targetSemesters - completedSemesters;
    if (remainSemesters <= 0) {
        return result;
    }

    // Gom lai khoa hoc sau khi da topo
    QStringList allCourses;
    for (const QStringList &layer : layers)
        for (const QString &c : layer)
            allCourses << c;

    int totalCourses = allCourses.size();
    if (totalCourses == 0)
        return result;
    QHash<QString, QStringList> prereqMap;
    for (int u = 0; u < m_courses.size(); ++u) {
        for (int v : m_adj[u]) {
            QString from = m_courses[u];
            QString to = m_courses[v];
            if (allCourses.contains(from) && allCourses.contains(to))
                prereqMap[to].append(from);
        }
    }

    QHash<QString, int> assignedSem; // save cac mon vao moi ki
    QSet<QString> completedSet = QSet<QString>(completedCourses.begin(), completedCourses.end());

    // Phan bo cac mon ra cac ki
    int base = totalCourses / remainSemesters;
    int extra = totalCourses % remainSemesters; // phan du

    int assignedCount = 0;
    QVector<bool> used(totalCourses, false);

    for (int i = 0; i < remainSemesters && assignedCount < totalCourses; ++i) {
        int semIndex = completedSemesters + i + 1;
        int take = base + (i < extra ? 1 : 0);

        QStringList semCourses;
        int taken = 0;
        bool progress = true;

        while (taken < take && progress) {
            progress = false;
            for (int j = 0; j < totalCourses; ++j) {
                if (used[j]) continue;
                const QString &c = allCourses[j];
                const QStringList &pres = prereqMap.value(c);

                bool ok = true;
                for (const QString &p : pres) {
                    if (completedSet.contains(p)) continue;
                    if (!assignedSem.contains(p) || assignedSem[p] >= semIndex) {
                        ok = false;
                        break;
                    }
                }

                if (ok) {
                    used[j] = true;
                    assignedSem[c] = semIndex;
                    semCourses << c;
                    taken++;
                    assignedCount++;
                    progress = true;
                    if (taken >= take)
                        break;
                }
            }
        }
        if (semCourses.isEmpty() && assignedCount < totalCourses) {
            for (int j = 0; j < totalCourses; ++j) {
                if (!used[j]) {
                    const QString &c = allCourses[j];
                    used[j] = true;
                    assignedSem[c] = semIndex;
                    semCourses << c;
                    assignedCount++;
                    break;
                }
            }
        }

        QVariantMap sem;
        sem["semester"] = semIndex;
        sem["courses"] = semCourses;
        result << sem;
    }

    // don mon du vao ki cuoi cung
    if (assignedCount < totalCourses && !result.isEmpty()) {
        QStringList rest;
        for (int j = 0; j < totalCourses; ++j)
            if (!used[j])
                rest << allCourses[j];

        QVariantMap last = result.takeLast().toMap();
        QStringList merged = last["courses"].toStringList() + rest;
        last["courses"] = merged;
        result << last;
    }
    return result;
}


// Xu li do thi
QString CourseGraph::dotFromCourses(const QStringList &courses) const {
    QString dot = "digraph prereq {\n"
                  "rankdir=LR;\n"
                  "node [shape=box, style=rounded, fontsize=10, margin=\"0.2,0.1\", width=0, height=0, fixedsize=false];\n"
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
