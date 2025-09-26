#ifndef COURSEGRAPH_H
#define COURSEGRAPH_H

#include <QObject>
#include <QStringList>
#include <QVector>
#include <QSet>
#include <QVariantList>

class CourseGraph : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList courses READ courses NOTIFY coursesChanged)

public:
    explicit CourseGraph(QObject *parent = nullptr);

    QStringList courses() const { return m_courses; }

    Q_INVOKABLE QStringList coursesForSemester(int semester) const;
    Q_INVOKABLE QVariantList availableSemesters() const;
    Q_INVOKABLE QStringList topoSort();
    Q_INVOKABLE QString svgBase64FromDot();
    Q_INVOKABLE QString svgForCourse(const QString &courseName);

signals:
    void coursesChanged();
    void errorOccurred(const QString &msg);

private:
    void buildSampleGraph();
    QString dotFromGraph() const;
    bool dfsTopoUtil(int v, QVector<int> &vis, QStringList &stack) const;

    QStringList m_courses;
    QVector<int> m_semesters;     // học kỳ của từng môn
    QVector<QVector<int>> m_adj;  // danh sách kề
    QString m_dot;
};

#endif // COURSEGRAPH_H
