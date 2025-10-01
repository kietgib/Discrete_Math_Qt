#pragma once
#include <QObject>
#include <QStringList>
#include <QVector>
#include <QSet>
#include <QVariant>
#include <QVariantList>


class CourseGraph : public QObject {
    Q_OBJECT
public:
    explicit CourseGraph(QObject *parent = nullptr);

    Q_PROPERTY(QStringList courses READ courses NOTIFY coursesChanged)

    Q_INVOKABLE QStringList coursesForSemester(int semester) const;
    Q_INVOKABLE QVariantList availableSemesters() const;
    Q_INVOKABLE QString svgForCourse(const QString &courseName);
    Q_INVOKABLE QStringList topoSort();

    QStringList courses() const;

    Q_INVOKABLE QString svgForCourses(const QStringList &courseNames);
    Q_INVOKABLE void setCourseSelected(const QString &course, bool selected);
signals:
    void coursesChanged();
    void errorOccurred(const QString &msg);

private:
    void buildSampleGraph();
    QString dotFromGraph() const;
    bool dfsTopoUtil(int v, QVector<int> &vis, QStringList &stack) const;

    QString svgBase64FromDot();
    QString m_dot;
    QStringList selectedCourses;

    QVector<QString> m_courses;
    QVector<int> m_semesters;
    QVector<QVector<int>> m_adj;
};
