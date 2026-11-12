import tkinter as tk
from pathlib import Path
from .theme import WHITE_SQUARE, BLACK_SQUARE, SELECTED, TARGET, CHECK

PIECE_FILE = {
    "king": "K", "queen": "Q", "rook": "R", "bishop": "B",
    "knight": "N", "pawn": "p",
}

COLOR_PREFIX = {"white": "w", "black": "b"}

_IMG_DIR = Path(__file__).resolve().parent.parent.parent / "img"


def load_piece_images(square_size):
    images = {}
    for color, prefix in COLOR_PREFIX.items():
        for ptype, letter in PIECE_FILE.items():
            path = _IMG_DIR / f"{prefix}{letter}.png"
            img = tk.PhotoImage(file=str(path))
            # scale to fit the square as best PhotoImage allows
            if img.width() > square_size or img.height() > square_size:
                factor = max(img.width(), square_size) // square_size
                img = img.subsample(factor, factor)
            images[(color, ptype)] = img
    return images


class BoardWidget(tk.Canvas):
    def __init__(self, parent, on_square_click, square_size=72, on_drag_drop=None, on_square_press=None):
        super().__init__(
            parent,
            width=8 * square_size,
            height=8 * square_size,
            highlightthickness=0,
        )
        self._on_square_click = on_square_click
        self._on_drag_drop = on_drag_drop
        self._on_square_press = on_square_press
        self._square_size = square_size
        self._piece_ids = {}
        self._images = load_piece_images(square_size)

        self.bind("<ButtonPress-1>", self._on_press)
        self.bind("<B1-Motion>", self._on_drag)
        self.bind("<ButtonRelease-1>", self._on_release)
        self._drag_start_square = None
        self._drag_piece_id = None
        self._drag_start_xy = None
        self._dragging = False
        self._draw_board()

    def _draw_board(self):
        for rank in range(8):
            for file in range(8):
                x0 = file * self._square_size
                y0 = rank * self._square_size
                x1 = x0 + self._square_size
                y1 = y0 + self._square_size
                color = WHITE_SQUARE if (rank + file) % 2 == 0 else BLACK_SQUARE
                self.create_rectangle(x0, y0, x1, y1, fill=color, outline="")

    def render(self, state, selected, targets, check_square=None, captures=None):
        self.delete("piece", "highlight")
        self._piece_ids.clear()

        if check_square:
            self._highlight(check_square, CHECK)
        if selected:
            self._highlight(selected, SELECTED)
        captures_set = set(captures or [])
        for t in (targets or []):
            if t in captures_set:
                self._highlight(t, TARGET)
            else:
                self._target_circle(t)

        board = state.get("board", "")
        rank = 7
        file = 0
        for ch in board:
            if ch == "/":
                rank -= 1
                file = 0
            elif ch.isdigit():
                file += int(ch)
            else:
                square = chr(ord("a") + file) + str(rank + 1)
                color = "white" if ch.isupper() else "black"
                ptype = {
                    "P": "pawn", "N": "knight", "B": "bishop",
                    "R": "rook", "Q": "queen", "K": "king",
                }[ch.upper()]
                img = self._images[(color, ptype)]
                x = file * self._square_size + self._square_size // 2
                y = (7 - rank) * self._square_size + self._square_size // 2
                pid = self.create_image(
                    x, y, image=img, tags=("piece",),
                )
                self._piece_ids[square] = pid
                file += 1

    def _highlight(self, square, color):
        file = ord(square[0]) - ord("a")
        rank = int(square[1]) - 1
        x0 = file * self._square_size
        y0 = (7 - rank) * self._square_size
        x1 = x0 + self._square_size
        y1 = y0 + self._square_size
        pid = self.create_rectangle(
            x0, y0, x1, y1, fill=color, stipple="gray50", tags=("highlight",),
        )

    def _square_center(self, square):
        file = ord(square[0]) - ord("a")
        rank = int(square[1]) - 1
        x = file * self._square_size + self._square_size // 2
        y = (7 - rank) * self._square_size + self._square_size // 2
        return x, y

    def _animate_items(self, items, duration=100, on_done=None):
        if not items:
            if on_done:
                on_done()
            return

        if getattr(self, "_anim_job", None) is not None:
            self.after_cancel(self._anim_job)
            self._anim_job = None

        steps = max(1, duration // 16)
        interval = max(1, duration // steps)

        def finish():
            for pid, sx, sy, ex, ey in items:
                self.coords(pid, ex, ey)
            self._anim_job = None
            if on_done:
                on_done()

        def step(i):
            if i >= steps:
                finish()
                return
            t = (i + 1) / steps
            for pid, sx, sy, ex, ey in items:
                self.coords(pid, sx + (ex - sx) * t, sy + (ey - sy) * t)
            self._anim_job = self.after(interval, lambda: step(i + 1))

        for pid, sx, sy, ex, ey in items:
            self.coords(pid, sx, sy)
        step(0)

    def animate_move(self, source, target, duration=100, on_done=None):
        pid = self._piece_ids.get(source)
        if pid is None:
            if on_done:
                on_done()
            return
        sx, sy = self._square_center(source)
        ex, ey = self._square_center(target)
        self._animate_items([(pid, sx, sy, ex, ey)], duration, on_done)

    def animate_moves(self, moves, duration=100, on_done=None):
        items = []
        for source, target in moves:
            pid = self._piece_ids.get(source)
            if pid is None:
                continue
            sx, sy = self._square_center(source)
            ex, ey = self._square_center(target)
            items.append((pid, sx, sy, ex, ey))
        self._animate_items(items, duration, on_done)

    def animate_drag(self, source, target, start_x, start_y, duration=100, on_done=None):
        pid = self._piece_ids.get(source)
        if pid is None:
            if on_done:
                on_done()
            return
        ex, ey = self._square_center(target)
        self._animate_items([(pid, start_x, start_y, ex, ey)], duration, on_done)

    def show_move_highlights(self, selected=None, targets=None, captures=None):
        self.delete("highlight")
        if selected:
            self._highlight(selected, SELECTED)
        captures_set = set(captures or [])
        for t in (targets or []):
            if t in captures_set:
                self._highlight(t, TARGET)
            else:
                self._target_circle(t)
        if self.find_withtag("piece"):
            try:
                self.tag_lower("highlight", "piece")
            except tk.TclError:
                pass

    def clear_highlights(self):
        self.delete("highlight")

    def _target_circle(self, square):
        file = ord(square[0]) - ord("a")
        rank = int(square[1]) - 1
        cx = file * self._square_size + self._square_size // 2
        cy = (7 - rank) * self._square_size + self._square_size // 2
        r = self._square_size // 5
        self.create_oval(
            cx - r, cy - r, cx + r, cy + r,
            fill="#c8c8c8", stipple="gray50", outline="", tags=("highlight",),
        )

    def _pixel_to_square(self, x, y):
        file = x // self._square_size
        rank = 7 - (y // self._square_size)
        if file < 0 or file > 7 or rank < 0 or rank > 7:
            return None
        return chr(ord("a") + file) + str(rank + 1)

    def _on_press(self, event):
        square = self._pixel_to_square(event.x, event.y)
        if not square:
            self._drag_start_square = None
            return
        pid = self._piece_ids.get(square)
        if pid is None:
            self._drag_start_square = None
            if self._on_square_press is not None:
                pass
            return
        self._drag_start_square = square
        self._drag_piece_id = pid
        self._drag_start_xy = (event.x, event.y)
        self._dragging = False
        self.tag_raise(pid)
        if self._on_square_press is not None:
            self._on_square_press(square)

    def _on_drag(self, event):
        if self._drag_start_square is None or self._drag_piece_id is None:
            return
        sx, sy = self._drag_start_xy
        if not self._dragging and ((event.x - sx) ** 2 + (event.y - sy) ** 2) ** 0.5 > 5:
            self._dragging = True
        if self._dragging:
            self.coords(self._drag_piece_id, event.x, event.y)

    def _on_release(self, event):
        start = self._drag_start_square
        pid = self._drag_piece_id
        dragging = self._dragging
        self._drag_start_square = None
        self._drag_piece_id = None
        self._drag_start_xy = None
        self._dragging = False

        end_square = self._pixel_to_square(event.x, event.y)
        if start is None:
            if end_square and self._on_square_click:
                self._on_square_click(end_square)
            return
        if dragging and end_square and end_square != start:
            if self._on_drag_drop is not None:
                ok = self._on_drag_drop(start, end_square, event.x, event.y)
                if not ok:
                    # snap back
                    x, y = self._square_center(start)
                    if pid is not None:
                        self.coords(pid, x, y)
            else:
                # fallback to click-to-move
                if self._on_square_click:
                    self._on_square_click(start)
                    self._on_square_click(end_square)
        else:
            # treat as click
            if end_square and self._on_square_click:
                self._on_square_click(end_square)
