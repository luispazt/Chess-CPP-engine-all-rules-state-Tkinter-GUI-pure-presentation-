import tkinter as tk
from tkinter import simpledialog


class PromotionDialog(simpledialog.Dialog):
    def __init__(self, parent, title="Promote pawn"):
        self.result = None
        super().__init__(parent, title)

    def body(self, master):
        tk.Label(master, text="Choose piece:").pack(pady=5)
        first = None
        for label, code in [("Queen", "q"), ("Rook", "r"), ("Bishop", "b"), ("Knight", "n")]:
            b = tk.Button(master, text=label, width=10,
                          command=lambda c=code: self._pick(c))
            b.pack(pady=2)
            if first is None:
                first = b
        return first

    def buttonbox(self):
        pass

    def _pick(self, code):
        self.result = code
        self.destroy()
