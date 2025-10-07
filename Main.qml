import QtQuick 2.15
import QtQuick.Controls 2.15
import App 1.0

ApplicationWindow {
    visible: true
    width: Screen.width
    height: Screen.height
    color: "#25558e"
    title: "Course Graph Viewer by QT"

    CourseGraph {
        id: graph
        onErrorOccurred: {
            msgDialog.text = msg
            msgDialog.open()
        }
    }

    property var completedCourses: []
    property int completedSemesters: 0
    property int targetSemesters: 8

    Column {
        anchors.fill: parent
        spacing: 8
        padding: 20

        // --- Logo + tiêu đề ---
        Row {
            spacing: 12
            height: 80

            Image {
                source: "qrc:/qml/logo.png"
                width: 75; height: 75
                fillMode: Image.PreserveAspectFit
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                Text { text: "Ho Chi Minh City University of Technology and Education"; font.pointSize: 8; font.bold: true; color: "#D3D3D3" }
                Text { text: "Faculty of International Education"; font.pointSize: 8; font.bold: true; color: "#D3D3D3" }
            }
        }

        Row {
            spacing: 50
            width: parent.width

            // --- Cột trái ---
            Column {
                width: parent.width * 0.25
                spacing: 8

                Text { text: "COURSE PLANNING"; font.pointSize: 26; font.bold: true; color:"#FFFFFF" }
                Text { text: "Program: Information Technology"; font.pointSize: 14; font.bold: true; color:"#FFFFFF" }

                // Completed / Target Semester
                Row {
                    spacing: 10
                    Label { text: "Completed Semesters:"; color: "white" }
                    SpinBox { from: 0; to: 6; value: 0; onValueChanged: completedSemesters = value }
                }
                Row {
                    spacing: 10
                    Label { text: "Target Semesters:"; color: "white" }
                    SpinBox { from: 7; to: 16; value: 8; onValueChanged: targetSemesters = value }
                }

                // --- Search ---
                TextField {
                    id: searchField
                    placeholderText: "Tìm môn học..."
                    width: parent.width
                    onTextChanged: {
                        searchModel.clear()
                        if (text.length > 0) {
                            var results = graph.searchCourse(text)
                            for (var i=0; i<results.length; i++) {
                                searchModel.append(results[i])
                            }
                        }
                    }
                }

                // --- List kết quả tìm kiếm ---
                ListView {
                    id: searchResults
                    width: parent.width
                    height: 200
                    clip: true
                    model: ListModel { id: searchModel }

                    delegate: Row {
                        spacing: 8
                        CheckBox {
                            id: chk
                            text: model.name
                            onCheckedChanged: {
                                if (checked) {
                                    if (completedCourses.indexOf(model.name) === -1)
                                        completedCourses.push(model.name)
                                } else {
                                    var idx = completedCourses.indexOf(model.name)
                                    if (idx !== -1) completedCourses.splice(idx, 1)
                                }
                            }
                        }
                        Text { text: "(Kỳ " + model.semester + ")"; color: "white"; font.pointSize: 10 }
                    }
                }

                // --- Danh sách môn đã tick ---
                Text { text: "Danh sách đã chọn:"; color: "yellow"; font.bold: true }
                ListView {
                    width: parent.width
                    height: 100
                    model: ListModel { id: completedModel }
                    delegate: Text { text: model.name; color: "white"; font.pointSize: 10 }
                }

                // --- Buttons ---
                Row {
                    spacing: 8
                    Button {
                        text: "Generate Plan"
                        onClicked: {
                            var plan = graph.generateStudyPlan(completedSemesters, targetSemesters, completedCourses)
                            outputModel.clear()
                            var remaining = []
                            for (var i=0; i<plan.length; i++) {
                                var sem = plan[i]
                                var semCourses = sem.courses
                                outputModel.append({ semester: "Semester " + sem.semester, courses: semCourses.join(", ") })
                                for (var j=0; j<semCourses.length; j++)
                                    remaining.push(semCourses[j])
                            }
                            svgImage.source = graph.svgForPlan(remaining)

                            completedModel.clear()
                            for (var k=0; k<completedCourses.length; k++) {
                                completedModel.append({ name: completedCourses[k] })
                            }
                        }
                    }
                    Button {
                        text: "Clear All"
                        onClicked: {
                            completedCourses = []
                            completedModel.clear()
                            searchModel.clear()
                            searchField.text = ""
                            svgImage.source = ""
                            outputModel.clear()
                        }
                    }
                }
            }

            // --- Cột phải ---
            Rectangle {
                width: parent.width * 0.7
                height: 650
                color: "#ffffff"
                border.color: "#cccccc"
                radius: 8

                Column {
                    anchors.fill: parent
                    spacing: 10
                    padding: 10

                    // Nửa trên: Output result
                    ListView {
                        width: parent.width
                        height: parent.height * 0.4
                        model: ListModel { id: outputModel }
                        clip: true
                        spacing: 6

                        delegate: Column {
                            width: ListView.view.width
                            spacing: 2

                            Text {
                                text: semester
                                font.bold: true
                                font.pointSize: 12
                                color: "#000000"
                                horizontalAlignment: Text.AlignLeft
                            }

                            Text {
                                text: courses
                                font.pointSize: 11
                                color: "#333333"
                                wrapMode: Text.Wrap   // Cho xuống dòng
                                width: parent.width - 20
                                horizontalAlignment: Text.AlignLeft
                                anchors.left: parent.left
                                anchors.leftMargin: 20
                                clip: true
                            }
                        }
                    }

                    // Nửa dưới: Graph
                    Flickable {
                        width: parent.width
                        height: parent.height * 0.55
                        contentWidth: svgImage.width * svgImage.scale
                        contentHeight: svgImage.height * svgImage.scale
                        clip: true

                        Image {
                            id: svgImage
                            anchors.centerIn: parent
                            source: ""
                            asynchronous: true
                            smooth: true
                            fillMode: Image.PreserveAspectFit
                            transformOrigin: Item.TopLeft
                            scale: 1.0
                            width: parent.width - 20
                            height: parent.height - 20
                        }
                    }
                }
            }
        }
    }

    // Error dialog
    Dialog {
        id: msgDialog
        title: "Notification"
        modal: true
        standardButtons: Dialog.Ok
        x: (parent ? (parent.width - width) / 2 : 0)
        y: (parent ? (parent.height - height) / 2 : 0)
        contentItem: Text {
            id: dialogText
            text: ""
            wrapMode: Text.Wrap
            padding: 12
        }
        property alias text: dialogText.text
    }
}
