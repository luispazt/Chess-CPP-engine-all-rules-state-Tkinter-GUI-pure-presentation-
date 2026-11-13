import tkinter as tk
from tkinter import messagebox
from .board_widget import BoardWidget
from .promotion_dialog import PromotionDialog
from engine_client import EngineClient


class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Chess")
        self.engine = EngineClient()
        self.board = BoardWidget(self, on_square_click=self._on_square_click, on_drag_drop=self._on_drag_drop, on_square_press=self._on_square_press)
        self.board.pack()
        ctrl = tk.Frame(self)
        ctrl.pack(fill="x")
        tk.Button(ctrl, text="Undo", command=self._on_undo).pack(side="left")
        self.status = tk.Label(self, text="", anchor="w")
        self.status.pack(fill="x")
        self._selected = None
        self._targets = []
        self._capture_targets = []
        self._animating = False
        self.bind("<Left>", lambda e: self._on_undo())
        self._refresh()

    def _on_undo(self):
        st = self.engine.state()
        if st["status"] not in ("in_progress", "check", "checkmate", "stalemate"):
            return
        self.engine.undo()
        self._selected, self._targets, self._capture_targets = None, [], []
        self._refresh()

    def _own_piece_at(self, square, state):
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
                if chr(ord("a") + file) + str(rank + 1) == square:
                    color = "white" if ch.isupper() else "black"
                    return color == state.get("turn")
                file += 1
        return False

    def _piece_char_at(self, square, state):
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
                if chr(ord("a") + file) + str(rank + 1) == square:
                    return ch
                file += 1
        return None

    def _prompt_promotion(self):
        dlg = PromotionDialog(self)
        return dlg.result or "q"

    def _on_square_click(self, square):
        if getattr(self, "_animating", False):
            return
        st = self.engine.state()
        if st["status"] not in ("in_progress", "check"):
            return

        animating = False
        if self._selected is None:
            if self._own_piece_at(square, st):
                self._selected = square
                self._targets = [u[2:4] for u in self.engine.legal_moves(square)]
                self._capture_targets = self._compute_captures(square, self._targets, st)
        elif square in self._targets:
            uci = self._selected + square
            piece_char = self._piece_char_at(self._selected, st)
            is_promo = (piece_char is not None and piece_char.lower() == "p" and square[1] in ("1", "8")) or \
                       (any(len(u) == 5 for u in self.engine.legal_moves(self._selected)) and square[1] in ("1", "8"))
            if is_promo:
                uci += self._prompt_promotion()
            if self.engine.try_move(uci):
                src = self._selected
                dst = square
                self._selected, self._targets, self._capture_targets = None, [], []
                self.board.delete("highlight")
                st2 = self.engine.state()
                self.status.config(text=f"{st2['turn'].title()} to move - {st2['status']}")
                moves = [(src, dst)]
                if abs(ord(src[0]) - ord(dst[0])) >= 2 and src[1] == dst[1]:
                    if dst[0] == "g":
                        moves.append(("h" + src[1], "f" + src[1]))
                    else:
                        moves.append(("a" + src[1], "d" + src[1]))
                self._animating = True
                self.board.animate_moves(moves, 100, on_done=self._on_animation_done)
                animating = True
        elif self._own_piece_at(square, st):
            self._selected = square
            self._targets = [u[2:4] for u in self.engine.legal_moves(square)]
            self._capture_targets = self._compute_captures(square, self._targets, st)
        else:
            self._selected, self._targets, self._capture_targets = None, [], []

        if not animating:
            self._refresh()

    def _on_square_press(self, square):
        st = self.engine.state()
        if st["status"] not in ("in_progress", "check"):
            return
        if not self._own_piece_at(square, st):
            return
        self._selected = square
        self._targets = [u[2:4] for u in self.engine.legal_moves(square)]
        self._capture_targets = self._compute_captures(square, self._targets, st)
        self.board.show_move_highlights(square, self._targets, self._capture_targets)

    def _compute_captures(self, square, targets, state):
        board = state.get("board", "")
        turn = state.get("turn")
        piece_char = None
        rank = 7
        file = 0
        for ch in board:
            if ch == "/":
                rank -= 1
                file = 0
            elif ch.isdigit():
                file += int(ch)
            else:
                if chr(ord("a") + file) + str(rank + 1) == square:
                    piece_char = ch
                file += 1
        is_pawn = piece_char is not None and piece_char.lower() == "p"
        captures = []
        for t in targets:
            occ_color = None
            rank = 7
            file = 0
            for ch in board:
                if ch == "/":
                    rank -= 1
                    file = 0
                elif ch.isdigit():
                    file += int(ch)
                else:
                    if chr(ord("a") + file) + str(rank + 1) == t:
                        occ_color = "white" if ch.isupper() else "black"
                    file += 1
            if occ_color is not None and occ_color != turn:
                captures.append(t)
            elif is_pawn and square[0] != t[0]:
                captures.append(t)
        return captures

    def _on_drag_drop(self, start, end, x, y):
        if getattr(self, "_animating", False):
            return False
        st = self.engine.state()
        if st["status"] not in ("in_progress", "check"):
            return False
        if not self._own_piece_at(start, st):
            return False

        targets = [u[2:4] for u in self.engine.legal_moves(start)]
        if end not in targets:
            return False

        uci = start + end
        board = st.get("board", "")
        rank = 7
        file = 0
        piece_char = None
        for ch in board:
            if ch == "/":
                rank -= 1
                file = 0
            elif ch.isdigit():
                file += int(ch)
            else:
                if chr(ord("a") + file) + str(rank + 1) == start:
                    piece_char = ch
                file += 1
        if (piece_char is not None and piece_char.lower() == "p" and end[1] in ("1", "8")) or \
                (any(len(u) == 5 for u in self.engine.legal_moves(start)) and end[1] in ("1", "8")):
            uci += self._prompt_promotion()

        if not self.engine.try_move(uci):
            return False

        self._selected, self._targets, self._capture_targets = None, [], []
        self.board.delete("highlight")
        st2 = self.engine.state()
        self.status.config(text=f"{st2['turn'].title()} to move - {st2['status']}")

        moves = [(start, end)]
        if abs(ord(start[0]) - ord(end[0])) >= 2 and start[1] == end[1]:
            if end[0] == "g":
                moves.append(("h" + start[1], "f" + start[1]))
            else:
                moves.append(("a" + start[1], "d" + start[1]))

        self._animating = True
        if len(moves) == 1:
            self.board.animate_drag(start, end, x, y, 100, on_done=self._on_animation_done)
        else:
            king_pid = self.board._piece_ids.get(start)
            rook_src = moves[1][0]
            rook_dst = moves[1][1]
            rook_pid = self.board._piece_ids.get(rook_src)
            ex, ey = self.board._square_center(end)
            rx, ry = self.board._square_center(rook_src)
            rdx, rdy = self.board._square_center(rook_dst)
            items = []
            if king_pid is not None:
                items.append((king_pid, x, y, ex, ey))
            if rook_pid is not None:
                items.append((rook_pid, rx, ry, rdx, rdy))
            self.board._animate_items(items, 100, on_done=self._on_animation_done)
        return True

    def _on_animation_done(self):
        self._animating = False
        self._refresh()

    def _refresh(self):
        if getattr(self, "_animating", False):
            job = getattr(self.board, "_anim_job", None)
            if job is not None:
                self.board.after_cancel(job)
                self.board._anim_job = None
            self._animating = False
        st = self.engine.state()
        check_square = None
        if st.get("inCheck"):
            turn = st.get("turn")
            board = st.get("board", "")
            rank = 7
            file = 0
            for ch in board:
                if ch == "/":
                    rank -= 1
                    file = 0
                elif ch.isdigit():
                    file += int(ch)
                else:
                    if ch.upper() == "K":
                        color = "white" if ch.isupper() else "black"
                        if color == turn:
                            check_square = chr(ord("a") + file) + str(rank + 1)
                    file += 1
        self.board.render(st, self._selected, self._targets, check_square, self._capture_targets)
        self.status.config(text=f"{st['turn'].title()} to move - {st['status']}")
