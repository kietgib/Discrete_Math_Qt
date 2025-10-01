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

    Column {
        anchors.fill: parent
        spacing: 8
        padding: 20

        // Logo trường
        Row {
            spacing: 12
            height: 80

            Image {
                source: "qrc:/qml/logo.png"
                width: 75
                height: 75
                fillMode: Image.PreserveAspectFit
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                Text {
                    text: "Ho Chi Minh City of University of Technology and Education"
                    font.pointSize: 8
                    font.bold: true
                    color: "#D3D3D3"
                }
                Text {
                    text: "Faculty of International Education"
                    font.pointSize: 8
                    lineHeightMode: Text.ProportionalHeight
                    font.bold: true
                    color: "#D3D3D3"
                }
            }
        }

        Row {
            spacing: 50
            width: parent.width

            // Cột trái
            Column {
                width: parent.width * 0.25
                spacing: 5

                Text { text: "COURSE PLANNING"; font.pointSize: 26; font.bold: true;color:"#FFFFFF" }
                Text { text: "Program: Information Technology";font.bold: true; font.pointSize: 14;color:"#FFFFFF" }

                Text { text: "Tran Gia Kiet - 24110103"; font.pointSize: 10; font.italic: true; font.family:"Arial";color:"#FFFFFF" }
                Text { text: "Mai Viet Thanh - 24110129"; font.pointSize: 10; font.italic: true; font.family:"Arial";color:"#FFFFFF" }
                Text { text: "Nguyen Huynh Minh Tuyet - 24110144"; font.pointSize: 10; font.italic: true; font.family:"Arial"; lineHeight: 3.0;color:"#FFFFFF"}

                // Semester
                Text { text: "Semester"; font.pointSize: 12; font.italic: true;font.bold:true;color:"#e6e600" }
                ComboBox {
                    id: semesterSelector
                    width: parent.width*0.7
                    model: graph.availableSemesters()

                    onActivated: {
                        courseSelector.model = graph.coursesForSemester(currentValue)
                        courseSelector.currentIndex = -1
                        svgImage.source = ""
                    }
                }

                Text { text: "Select multiple courses"; font.pointSize: 12; font.italic: true;font.bold:true;color:"#F5FFC2" }
                ListView {
                    id: multiCourseList
                    width: parent.width
                    height: 300
                    model: graph.coursesForSemester(semesterSelector.currentValue)
                    clip: true

                    delegate: Item {
                        width: multiCourseList.width
                        height: 34

                        Row {
                            anchors.fill: parent
                            anchors.margins: 4
                            spacing: 5

                            CheckBox {
                                id: checkBox
                                text: ""
                                anchors.verticalCenter: parent.verticalCenter

                                onCheckedChanged: {
                                    graph.setCourseSelected(modelData, checked)
                                }
                            }

                            Text {
                                id: label
                                text: modelData
                                color: "white"
                                font.pointSize: 10
                                verticalAlignment: Text.AlignVCenter
                                anchors.verticalCenter: parent.verticalCenter
                                elide: Text.ElideRight
                                wrapMode: Text.NoWrap
                                width: parent.width - checkBox.implicitWidth - 24
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (mouse.x > checkBox.implicitWidth + 12) {
                                    checkBox.checked = !checkBox.checked
                                }
                            }
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                        }
                        property alias checked: checkBox.checked
                        property alias text: label.text
                    }
                }





                // Buttons
                Row {
                    spacing: 8
                    Button {
                        text: "Add All"
                        onClicked: {
                            for (var i = 0; i < multiCourseList.count; ++i) {
                                var item = multiCourseList.itemAtIndex(i)
                                if (item) item.checked = true
                            }
                        }
                    }
                    Button {
                        text: "Clear All"
                        onClicked: {
                            for (var i = 0; i < multiCourseList.count; ++i) {
                                var item = multiCourseList.itemAtIndex(i)
                                if (item) item.checked = false
                            }
                            svgImage.source = ""
                        }
                    }
                    Button {
                        text: "Draw Graph"
                        onClicked: {
                            var selected = []
                            for (var i = 0; i < multiCourseList.count; ++i) {
                                var item = multiCourseList.itemAtIndex(i)
                                if (item && item.checked) selected.push(item.text)
                            }
                            var uri = graph.svgForCourses(selected)
                            if (uri && uri.length > 0) {
                                svgImage.source = uri
                            }
                        }
                    }
                }

        }

            // Màn vẽ đồ thị
            Rectangle {
                width: parent.width * 0.68
                height: 650
                color: "#ffffff"
                border.color: "#cccccc"
                radius: 8

                Flickable {
                    id: flick
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
                        scale: zoomArea.zoom

                        property real baseWidth: parent.width - 20
                        property real baseHeight: parent.height - 20
                        width: baseWidth
                        height: baseHeight
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
