import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import com.github.Sarcasm.fzy 1.0

Window {
    visible: true

    minimumWidth: 200
    minimumHeight: 300
    width: 500
    height: 600
    x: Screen.width / 2 - width / 2
    y: Screen.height / 2 - height / 2

    Shortcut {
        sequence: StandardKey.Quit
        context: Qt.ApplicationShortcut
        onActivated: Qt.quit()
    }

    ColumnLayout {
        anchors.fill: parent

        TextField {
            id: textField
            Layout.fillWidth: true
            focus: true
            placeholderText: qsTr("Filter...")
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            clip: true


            model: FzyModel {
                filter: textField.text
            }
            delegate: Text {
                id: name
                height: 30
                text: value
            }
        }
    }
}
