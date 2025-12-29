from PyQt6.QtWidgets import QApplication, QWidget, QGridLayout, QPushButton, QStyle
from PyQt6.QtCore import Qt

app = QApplication([])
w = QWidget()
layout = QGridLayout(w)
icons = [attr for attr in dir(QStyle.StandardPixmap) if attr.startswith('SP_')]

for i, name in enumerate(icons):
    btn = QPushButton(name)
    icon = app.style().standardIcon(getattr(QStyle.StandardPixmap, name))
    btn.setIcon(icon)
    layout.addWidget(btn, i // 4, i % 4)

w.show()
app.exec()