import QtQuick 2.15
import QtQuick.Controls 2.15
import App 1.0

ApplicationWindow {
    width: 1920
    height: 1080
    visible: true
    title: "Course Planner"

    CourseGraph {
        id: graph
    }

    property int completedSemesters: 1
    property int targetSemesters: 8
    property var completedCourses: []

    Column {
        anchors.centerIn: parent
        spacing: 12

        Row {
            spacing: 20
            Label { text: "Completed Semesters:" }
            SpinBox { from: 0; to: 6; value: 0; onValueChanged: completedSemesters = value }
            Label { text: "Target Semesters:" }
            SpinBox { from: 7; to: 16; value: 8; onValueChanged: targetSemesters = value }
        }

        Button {
            text: "Generate Plan"
            onClicked: {
                var plan = graph.generateStudyPlan(completedSemesters, targetSemesters, completedCourses)
                outputModel.clear()
                var remaining = []
                for (var i=0; i<plan.length; i++) {
                    var sem = plan[i]
                    outputModel.append({ semester: "Semester " + (completedSemesters + i + 1), courses: Array.from(sem.courses).join(", ")})
                    for (var j=0; j<sem.length; j++)
                        remaining.push(sem[j])
                }
                svgImg.source = graph.svgForPlan(remaining)
            }
        }

        ListView {
            width: parent.width * 0.9
            height: 200
            model: ListModel { id: outputModel }
            delegate: Rectangle {
                width: parent.width
                height: 30
                Text { anchors.centerIn: parent; text: semester + ": " + courses }
            }
        }

        Image {
            id: svgImg
            width: parent.width*0.9
            height: 300
            fillMode: Image.PreserveAspectFit
        }
    }
}
