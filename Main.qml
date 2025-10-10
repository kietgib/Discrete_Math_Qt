import QtQuick 2.15
import QtQuick.Controls 2.15
import App 1.0

ApplicationWindow {
    visible: true
    width: Screen.width
    height: Screen.height
    color: "#25558e"
    title: "Study Planner by Qt Creator"

    CourseGraph {
        id: graph
        onErrorOccurred: {
            msgDialog.text = msg
            msgDialog.open()
        }
    }
    // goi ham tu dong cap nhat sau moi lan thay doi lua chon
    function updatePlan() {
        var plan = graph.generateStudyPlan(completedSemesters, targetSemesters, completedCourses)
        outputModel.clear()
        var remaining = []
        for (var i=0; i<plan.length; i++) {
            var sem = plan[i]
            var semCourses = sem.courses
            outputModel.append({
                semester: "Semester " + sem.semester,
                courses: semCourses.join(", ")
            })
            for (var j=0; j<semCourses.length; j++)
                remaining.push(semCourses[j])
        }
        svgImage.source = graph.svgForPlan(remaining)
        completedModel.clear()
        for (var k=0; k<completedCourses.length; k++) {
            completedModel.append({ name: completedCourses[k] })
        }
    }
    property var completedCourses: []   // Goi them mang de xuong cho UI
    property int completedSemesters: 0
    property int targetSemesters: 8

    Column {
        anchors.fill: parent
        spacing: 8
        padding: 20

        // Logo + tiêu đề
        Rectangle {
               width: parent.width / 2
               height: 80
               color: "transparent"

                Row {
                    anchors.fill: parent
                    spacing: 12

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
            }

        Row {
            spacing: 50
            width: parent.width

            // Column trai
            Column {
                width: parent.width * 0.25


                Text { text: "COURSE PLANNING"; font.pointSize: 26; font.bold: true; color:"#FFFFFF" }
                Text { text: "Program: Information Technology"; font.pointSize: 14; font.bold: true; color:"#FFFFFF" }

                Row {
                spacing: 3
                Text { text: "Trần Gia Kiệt - 24110103"; font.pointSize: 9;  color:"#FFFFFF" ; font.italic: true}
                }
                Row
                {
                spacing: 3
                Text { text: "Mai Viết Thành - 24110129"; font.pointSize: 9;  color:"#FFFFFF" ; font.italic: true}
                }
                Row
                {
                spacing: 3
                Text { text: "Nguyễn Huỳnh Minh Tuyết - 24110144"; font.pointSize: 9;  color:"#FFFFFF" ; font.italic: true; lineHeight: 4}
                }

                spacing: 8

                // Yeu cau cua sv
                Row {
                    spacing: 10
                    Label { text: "Completed Semesters:"; font.bold: true; color: "white" }
                    SpinBox { from: 0; to: 6; value: 0;
                        onValueChanged: {
                            completedSemesters = value
                            updatePlan()
                        }
                    }
                }
                Row {
                    spacing: 10
                    Label { text: "Target Semesters:"; font.bold: true; color: "white" }
                    SpinBox { from: 7; to: 16; value: 8;
                        onValueChanged: {
                            targetSemesters = value
                            updatePlan()
                        }
                    }
                }

                // Search
                TextField {
                    id: searchField
                    placeholderText: "Finding subjects..."
                    width: parent.width * 0.75
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

                // listsearch
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
                            checked: completedCourses.indexOf(model.name) !== -1
                            // Không dùng text ở đây nữa để tránh bị đè
                            onCheckedChanged: {
                                if (checked) {
                                    if (completedCourses.indexOf(model.name) === -1)
                                        completedCourses.push(model.name)
                                } else {
                                    var idx = completedCourses.indexOf(model.name)
                                    if (idx !== -1) completedCourses.splice(idx, 1)
                                }
                                updatePlan()
                            }
                        }

                        Text {
                            text: model.name
                            color: "white"
                            verticalAlignment: Text.AlignVCenter
                        }

                        Text {
                            text: "(semester " + model.semester + ")"
                            font.bold: true
                            color: "white"
                            font.pointSize: 10
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                // Phần tick môn
                Text { text: "CHOSEN SUBJECTS:"; color: "yellow"; font.bold: true; font.pointSize:11 ; lineHeight: 1}
                ListView {
                    width: parent.width
                    height: 100
                    model: ListModel { id: completedModel }
                    delegate: Text { text: model.name; color: "white"; font.pointSize: 10 }
                }

                // Nut tao do thi
                Row {
                    spacing: 15
                    Button {
                        text: "Generate Plan"
                        onClicked: {
                            var plan = graph.generateStudyPlan(completedSemesters, targetSemesters, completedCourses)
                            outputModel.clear()
                            var remaining = []
                            for (var i=0; i<plan.length; i++) {
                                var sem = plan[i]
                                var semCourses = sem.courses
                                outputModel.append({
                                    semester: "Semester " + sem.semester,
                                    courses: semCourses.join(", ")
                                })
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

            // phan phai
            Rectangle {
                id: whiteBoardContainer
                width: parent.width * 0.68
                height: 650


                // Bảng trắng chính
                Rectangle {
                    id: whiteBoard
                    anchors.fill: parent
                    color: "#F0F0F0"
                    border.color: "#cccccc"
                    radius: 15

                    Column {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 15

                        // Output kq
                        ListView {
                            width: parent.width
                            height: parent.height * 0.4
                            model: ListModel { id: outputModel }
                            clip: true
                            spacing: 3

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
                                    wrapMode: Text.Wrap
                                    width: parent.width - 20
                                    horizontalAlignment: Text.AlignLeft
                                    anchors.left: parent.left
                                    anchors.leftMargin: 20
                                    clip: true
                                }
                            }
                        }

                        // Graphic
                        Rectangle {
                            id: graphBoard
                            width: parent.width
                            height: parent.height * 0.6
                            color: "#ffffff"
                            radius: 10

                            Flickable {
                                anchors.fill: parent
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
                                    scale: 1
                                }
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
}
