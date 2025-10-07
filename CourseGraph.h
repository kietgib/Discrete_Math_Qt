#pragma once
#include <QObject>
#include <QStringList>
#include <QVector>
#include <QVariantList>
#include <QSet>

class CourseGraph : public QObject {
    Q_OBJECT
public:
    explicit CourseGraph(QObject *parent = nullptr);

    Q_INVOKABLE QVariantList generateStudyPlan(
        int completedSemesters,
        int targetSemesters,
        const QStringList &completedCourses);

    Q_INVOKABLE QString svgForPlan(const QStringList &remainingCourses);
    Q_INVOKABLE QStringList topoSort();
signals:
    void errorOccurred(const QString &msg);

private:
    void buildSampleGraph();
    QString svgBase64FromDot(const QString &dot) const;
    QString dotFromCourses(const QStringList &courses) const;

    QVector<QString> m_courses;
    QVector<QVector<int>> m_adj; // adjacency list
};
