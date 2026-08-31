#include <libqt6c.h>

typedef enum {
    NO_SHAPE,
    Z_SHAPE,
    S_SHAPE,
    LINE_SHAPE,
    T_SHAPE,
    SQUARE_SHAPE,
    L_SHAPE,
    MIRRORED_L_SHAPE,
} TetrixShape;

#define NUM_SHAPES 8
#define NUM_CELLS 4
#define PAIR_CELLS 2
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 22

typedef struct {
    int coords[NUM_CELLS][PAIR_CELLS];
    TetrixShape piece_shape;
} TetrixPiece;

typedef struct {
    QFrame* frame;
    TetrixShape board[BOARD_WIDTH * BOARD_HEIGHT];
    QBasicTimer* timer;
    QLabel* next_piece_label;
    bool is_started;
    bool is_paused;
    bool is_waiting_after_line;
    TetrixPiece cur_piece;
    TetrixPiece next_piece;
    int16_t cur_x;
    int16_t cur_y;
    uint16_t num_lines_removed;
    uint16_t num_pieces_dropped;
    uint32_t score;
    uint16_t level;
} TetrixBoard;

typedef struct {
    QWidget* window;
    TetrixBoard board;
    QLabel* next_piece_label;
    QLCDNumber* score_lcd;
    QLCDNumber* level_lcd;
    QLCDNumber* lines_lcd;
    QPushButton* new_game_button;
    QPushButton* quit_button;
    QPushButton* pause_button;
    QLabel* game_over_label;
} TetrixWindow;

static TetrixBoard global_board;
static TetrixWindow tetrix_window;
static int frame_width;

void set_x(TetrixPiece* self, int index, int X) {
    self->coords[index][0] = X;
}

void set_y(TetrixPiece* self, int index, int Y) {
    self->coords[index][1] = Y;
}

int get_x(const TetrixPiece* self, int index) {
    return self->coords[index][0];
}

int get_y(const TetrixPiece* self, int index) {
    return self->coords[index][1];
}

void set_shape(TetrixPiece* self, TetrixShape shape) {
    int coords_table[NUM_SHAPES][NUM_CELLS][PAIR_CELLS] = {
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, -1}, {0, 0}, {-1, 0}, {-1, 1}},
        {{0, -1}, {0, 0}, {1, 0}, {1, 1}},
        {{0, -1}, {0, 0}, {0, 1}, {0, 2}},
        {{-1, 0}, {0, 0}, {1, 0}, {0, 1}},
        {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
        {{-1, -1}, {0, -1}, {0, 0}, {0, 1}},
        {{1, -1}, {0, -1}, {0, 0}, {0, 1}},
    };

    for (int i = 0; i < NUM_CELLS; i++)
        for (int j = 0; j < PAIR_CELLS; j++)
            self->coords[i][j] = coords_table[shape][i][j];

    self->piece_shape = shape;
}

void set_random_shape(TetrixPiece* self) {
    set_shape(self, q_randomgenerator_bounded2((void*)q_randomgenerator_global(),
                                               NUM_SHAPES - 1) +
                        1);
}

int min_x(const TetrixPiece* self) {
    int min = self->coords[0][0];
    for (size_t i = 1; i < NUM_CELLS; i++)
        min = min < self->coords[i][0] ? min : self->coords[i][0];
    return min;
}

int max_x(const TetrixPiece* self) {
    int max = self->coords[0][0];
    for (size_t i = 1; i < NUM_CELLS; i++)
        max = max > self->coords[i][0] ? max : self->coords[i][0];
    return max;
}

int min_y(const TetrixPiece* self) {
    int min = self->coords[0][1];
    for (size_t i = 1; i < NUM_CELLS; i++)
        min = min < self->coords[i][1] ? min : self->coords[i][1];
    return min;
}

int max_y(const TetrixPiece* self) {
    int max = self->coords[0][1];
    for (size_t i = 1; i < NUM_CELLS; i++)
        max = max > self->coords[i][1] ? max : self->coords[i][1];
    return max;
}

void rotated_left(TetrixPiece* self) {
    if (self->piece_shape == SQUARE_SHAPE)
        return;

    for (int i = 0; i < NUM_CELLS; i++) {
        int x = get_x(self, i);
        int y = get_y(self, i);
        set_x(self, i, y);
        set_y(self, i, -x);
    }
}

void rotated_right(TetrixPiece* self) {
    if (self->piece_shape == SQUARE_SHAPE)
        return;

    for (int i = 0; i < NUM_CELLS; i++) {
        int x = get_x(self, i);
        int y = get_y(self, i);
        set_x(self, i, -y);
        set_y(self, i, x);
    }
}

void clear_board(TetrixBoard* self) {
    for (int i = 0; i < BOARD_HEIGHT * BOARD_WIDTH; i++)
        self->board[i] = NO_SHAPE;
}

int16_t timeout_time(const TetrixBoard* self) {
    return 1000 / (self->level + 1);
}

int square_width(const TetrixBoard* self) {
    QRect* rect = q_frame_contents_rect(self->frame);
    int ret = q_rect_width(rect) / BOARD_WIDTH;
    q_rect_delete(rect);
    return ret;
}

int square_height(const TetrixBoard* self) {
    QRect* rect = q_frame_contents_rect(self->frame);
    int ret = q_rect_height(rect) / BOARD_HEIGHT;
    q_rect_delete(rect);
    return ret;
}

TetrixShape shape_at(const TetrixBoard* self, int x, int y) {
    return self->board[y * BOARD_WIDTH + x];
}

QSize* on_size_hint() {
    return q_size_new4(BOARD_WIDTH * 15 + frame_width * 2,
                       BOARD_HEIGHT * 15 + frame_width * 2);
}

QSize* on_minimum_size_hint() {
    return q_size_new4(BOARD_WIDTH * 5 + frame_width * 2,
                       BOARD_HEIGHT * 5 + frame_width * 2);
}

bool try_move(TetrixBoard* self, TetrixPiece* new_piece, int16_t new_x, int16_t new_y) {
    for (int i = 0; i < NUM_CELLS; i++) {
        int16_t x = new_x + get_x(new_piece, i);
        int16_t y = new_y - get_y(new_piece, i);
        if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT)
            return false;
        if (shape_at(self, x, y) != NO_SHAPE)
            return false;
    }

    self->cur_piece = *(new_piece);
    self->cur_x = new_x;
    self->cur_y = new_y;
    q_frame_update(self->frame);
    return true;
}

void draw_square(const TetrixBoard* self, QPainter* painter, int x, int y, TetrixShape shape) {
    uint32_t color_table[8] = {
        0x000000,
        0xCC6666,
        0x66CC66,
        0x6666CC,
        0xCCCC66,
        0xCC66CC,
        0x66CCCC,
        0xDAAA00,
    };

    QColor* color = q_color_new6(color_table[shape]);
    QColor* lighter = q_color_lighter(color);
    QColor* darker = q_color_darker(color);

    q_painter_fill_rect5(painter, x + 1, y + 1, square_width(self) - 2, square_height(self) - 2, color);
    q_painter_set_pen(painter, lighter);
    q_painter_draw_line3(painter, x, y + square_height(self) - 1, x, y);
    q_painter_draw_line3(painter, x, y, x + square_width(self) - 1, y);

    q_painter_set_pen(painter, darker);
    q_painter_draw_line3(painter, x + 1, y + square_height(self) - 1, x + square_width(self) - 1,
                         y + square_height(self) - 1);
    q_painter_draw_line3(painter, x + square_width(self) - 1, y + square_height(self) - 1,
                         x + square_width(self) - 1, y + 1);

    q_color_delete(color);
    q_color_delete(lighter);
    q_color_delete(darker);
}

void show_next_piece(const TetrixBoard* self) {
    if (self->next_piece_label == NULL)
        return;

    int dx = max_x(&self->next_piece) - min_x(&self->next_piece) + 1;
    int dy = max_y(&self->next_piece) - min_y(&self->next_piece) + 1;

    QPixmap* pixmap = q_pixmap_new2(dx * square_width(self), dy * square_height(self));
    QPainter* painter = q_painter_new2(pixmap);
    QRect* rect = q_pixmap_rect(pixmap);

    q_painter_fill_rect3(painter, rect,
                         (void*)q_palette_window((void*)q_label_palette(self->next_piece_label)));

    for (int i = 0; i < NUM_CELLS; i++) {
        int x = get_x(&self->next_piece, i) - min_x(&self->next_piece);
        int y = get_y(&self->next_piece, i) - min_y(&self->next_piece);
        draw_square(self, painter, x * square_width(self),
                    y * square_height(self), self->next_piece.piece_shape);
    }

    q_label_set_pixmap(self->next_piece_label, pixmap);
    q_rect_delete(rect);
    q_painter_delete(painter);
    q_pixmap_delete(pixmap);
}

void new_piece(TetrixBoard* self) {
    self->cur_piece = self->next_piece;
    set_random_shape(&self->next_piece);
    show_next_piece(self);
    self->cur_x = BOARD_WIDTH / 2 + 1;
    self->cur_y = BOARD_HEIGHT - 1 + min_y(&self->cur_piece);

    if (!try_move(self, &self->cur_piece, self->cur_x, self->cur_y)) {
        set_shape(&self->cur_piece, NO_SHAPE);
        q_basictimer_stop(self->timer);
        self->is_started = false;
        q_label_show(tetrix_window.game_over_label);
        q_pushbutton_set_disabled(tetrix_window.pause_button, true);
    }
}

void remove_full_lines(TetrixBoard* self) {
    int num_full_lines = 0;
    for (int i = BOARD_HEIGHT - 1; i >= 0; i--) {
        int line_full = true;
        for (int j = 0; j < BOARD_WIDTH; j++)
            if (shape_at(self, j, i) == NO_SHAPE) {
                line_full = false;
                break;
            }
        if (line_full) {
            num_full_lines++;
            for (int k = i; k < BOARD_HEIGHT - 1; k++)
                for (int j = 0; j < BOARD_WIDTH; j++)
                    self->board[k * BOARD_WIDTH + j] = shape_at(self, j, k + 1);
            for (int j = 0; j < BOARD_WIDTH; j++)
                self->board[(BOARD_HEIGHT - 1) * BOARD_WIDTH + j] = NO_SHAPE;
        }
    }

    if (num_full_lines > 0) {
        self->num_lines_removed += num_full_lines;
        self->score += 10 * num_full_lines;
        q_lcdnumber_display2(tetrix_window.lines_lcd, self->num_lines_removed);
        q_lcdnumber_display2(tetrix_window.score_lcd, self->score);

        q_basictimer_start(self->timer, 500, self->frame);
        self->is_waiting_after_line = true;
        set_shape(&self->cur_piece, NO_SHAPE);
        q_frame_update(self->frame);
    }
}

void piece_dropped(TetrixBoard* self, uint8_t drop_height) {
    for (int i = 0; i < NUM_CELLS; i++) {
        int x = self->cur_x + get_x(&self->cur_piece, i);
        int y = self->cur_y - get_y(&self->cur_piece, i);
        self->board[y * BOARD_WIDTH + x] = self->cur_piece.piece_shape;
    }

    self->num_pieces_dropped++;
    if (self->num_pieces_dropped % 25 == 0) {
        self->level += 1;
        q_basictimer_start(self->timer, timeout_time(self), self->frame);
        q_lcdnumber_display2(tetrix_window.level_lcd, self->level);
    }

    self->score += drop_height + 7;
    q_lcdnumber_display2(tetrix_window.score_lcd, self->score);
    remove_full_lines(self);

    if (!self->is_waiting_after_line)
        new_piece(self);
}

void one_line_down(TetrixBoard* self) {
    if (!try_move(self, &self->cur_piece, self->cur_x, self->cur_y - 1))
        piece_dropped(self, 0);
}

void drop_down(TetrixBoard* self) {
    uint8_t drop_height = 0;
    int16_t new_y = self->cur_y;
    while (new_y > 0) {
        if (!try_move(self, &self->cur_piece, self->cur_x, new_y - 1))
            break;
        new_y -= 1;
        drop_height += 1;
    }
    piece_dropped(self, drop_height);
}

void on_timer_event(void* self, void* event) {
    if (q_timerevent_timer_id(event) == q_basictimer_timer_id(global_board.timer))
        if (global_board.is_waiting_after_line) {
            global_board.is_waiting_after_line = false;
            new_piece(&global_board);
            q_basictimer_start(global_board.timer, timeout_time(&global_board), self);
        } else
            one_line_down(&global_board);
    else
        q_frame_super_timer_event(self, event);
}

void on_key_press_event(void* self, void* event) {
    if (!global_board.is_started || global_board.is_paused ||
        global_board.cur_piece.piece_shape == NO_SHAPE) {
        q_frame_super_key_press_event(self, event);
        return;
    }

    switch (q_keyevent_key(event)) {
    case QT_KEY_Key_Left:
        try_move(&global_board, &global_board.cur_piece, global_board.cur_x - 1,
                 global_board.cur_y);
        break;
    case QT_KEY_Key_Right:
        try_move(&global_board, &global_board.cur_piece, global_board.cur_x + 1,
                 global_board.cur_y);
        break;
    case QT_KEY_Key_Down:
        if (global_board.cur_x == 0 || global_board.cur_x >= BOARD_WIDTH - 1)
            return;
        if (global_board.cur_piece.piece_shape == LINE_SHAPE &&
                global_board.cur_x <= 1 ||
            global_board.cur_x >= BOARD_WIDTH - 2)
            return;

        rotated_right(&global_board.cur_piece);
        try_move(&global_board, &global_board.cur_piece, global_board.cur_x,
                 global_board.cur_y);
        break;
    case QT_KEY_Key_Up:
        if (global_board.cur_x == 0 || global_board.cur_x >= BOARD_WIDTH - 1)
            return;
        if (global_board.cur_piece.piece_shape == LINE_SHAPE &&
                global_board.cur_x <= 1 ||
            global_board.cur_x >= BOARD_WIDTH - 2)
            return;

        rotated_left(&global_board.cur_piece);
        try_move(&global_board, &global_board.cur_piece, global_board.cur_x,
                 global_board.cur_y);
        break;
    case QT_KEY_Key_Space:
        drop_down(&global_board);
        break;
    case QT_KEY_Key_D:
        one_line_down(&global_board);
        break;
    default:
        q_frame_super_key_press_event(self, event);
        return;
    }
}

void on_paint_event(void* self, void* event) {
    q_frame_super_paint_event(self, event);

    QPainter* painter = q_painter_new2(q_frame_as_q_paint_device(self));
    QRect* rect = q_frame_contents_rect(self);

    if (global_board.is_paused) {
        q_painter_draw_text6(painter, rect, QT_ALIGNMENTFLAG_ALIGNCENTER,
                             "Pause");
        q_rect_delete(rect);
        q_painter_delete(painter);
        return;
    }

    int board_top = q_rect_bottom(rect) - BOARD_HEIGHT * square_height(&global_board);

    for (int i = 0; i < BOARD_HEIGHT; i++)
        for (int j = 0; j < BOARD_WIDTH; j++) {
            TetrixShape shape = shape_at(&global_board, j, BOARD_HEIGHT - i - 1);
            if (shape != NO_SHAPE)
                draw_square(&global_board, (QPainter*)painter,
                            q_rect_left(rect) + j * square_width(&global_board),
                            board_top + i * square_height(&global_board), shape);
        }

    if (global_board.cur_piece.piece_shape != NO_SHAPE)
        for (int i = 0; i < NUM_CELLS; i++) {
            int x = global_board.cur_x + get_x(&global_board.cur_piece, i);
            int y = global_board.cur_y - get_y(&global_board.cur_piece, i);
            draw_square(&global_board, (QPainter*)painter,
                        q_rect_left(rect) + x * square_width(&global_board),
                        board_top + (BOARD_HEIGHT - y - 1) * square_height(&global_board),
                        global_board.cur_piece.piece_shape);
        }

    q_rect_delete(rect);
    q_painter_delete(painter);
}

void initialize_board(TetrixBoard* self) {
    self->frame = q_frame_new2();
    q_frame_set_frame_style(self->frame, QFRAME_SHAPE_PANEL | QFRAME_SHADOW_SUNKEN);
    q_frame_set_focus_policy(self->frame, QT_FOCUSPOLICY_STRONGFOCUS);
    clear_board(self);

    set_shape(&self->cur_piece, NO_SHAPE);
    self->cur_x = 0;
    self->cur_y = 0;
    self->is_started = false;
    self->is_paused = false;

    set_random_shape(&self->next_piece);
    self->next_piece_label = NULL;

    frame_width = q_frame_frame_width(self->frame);
    self->timer = q_basictimer_new();

    q_frame_on_size_hint(self->frame, on_size_hint);
    q_frame_on_minimum_size_hint(self->frame, on_minimum_size_hint);
    q_frame_on_paint_event(self->frame, on_paint_event);
    q_frame_on_key_press_event(self->frame, on_key_press_event);
    q_frame_on_timer_event(self->frame, on_timer_event);
}

void cleanup_board(const TetrixBoard* self) {
    q_basictimer_delete(self->timer);
    q_frame_delete(self->frame);
}

void on_score_changed(void* self UNUSED, int value) {
    q_lcdnumber_display2(tetrix_window.score_lcd, value);
}

void on_level_changed(void* self UNUSED, int value) {
    q_lcdnumber_display2(tetrix_window.level_lcd, value);
}

void on_lines_removed_changed(void* self UNUSED, int value) {
    q_lcdnumber_display2(tetrix_window.lines_lcd, value);
}

QLabel* create_label(const char* text) {
    QLabel* label = q_label_new3(text);
    q_label_set_alignment(label, QT_ALIGNMENTFLAG_ALIGNHCENTER |
                                     QT_ALIGNMENTFLAG_ALIGNBOTTOM);
    return label;
}

void on_pause_activated(void* self UNUSED) {
    q_pushbutton_click(tetrix_window.pause_button);
}

void on_quit_activated(void* self UNUSED) {
    q_pushbutton_click(tetrix_window.quit_button);
}

void on_new_game_activated(void* self UNUSED) {
    q_pushbutton_click(tetrix_window.new_game_button);
}

void quit_game(void* self UNUSED) {
    q_application_quit();
}

void pause_game(void* self UNUSED) {
    if (!global_board.is_started)
        return;

    global_board.is_paused = !global_board.is_paused;
    if (global_board.is_paused)
        q_basictimer_stop(global_board.timer);
    else
        q_basictimer_start(global_board.timer, timeout_time(&global_board),
                           global_board.frame);

    q_frame_update(global_board.frame);
}

void new_game(void* self UNUSED) {
    if (global_board.is_paused)
        return;

    global_board.is_started = true;
    global_board.is_waiting_after_line = false;
    global_board.num_lines_removed = 0;
    global_board.num_pieces_dropped = 0;
    global_board.score = 0;
    global_board.level = 1;
    q_label_hide(tetrix_window.game_over_label);
    q_pushbutton_set_disabled(tetrix_window.pause_button, false);
    clear_board(&global_board);

    q_lcdnumber_display2(tetrix_window.lines_lcd, global_board.num_lines_removed);
    q_lcdnumber_display2(tetrix_window.score_lcd, global_board.score);
    q_lcdnumber_display2(tetrix_window.level_lcd, global_board.level);

    new_piece(&global_board);
    q_basictimer_start(global_board.timer, timeout_time(&global_board), global_board.frame);
}

void initialize_tetrix_window(TetrixWindow* self) {
    initialize_board(&self->board);
    self->next_piece_label = q_label_new2();
    q_label_set_frame_style(self->next_piece_label, QFRAME_SHAPE_BOX | QFRAME_SHADOW_RAISED);
    q_label_set_alignment(self->next_piece_label, QT_ALIGNMENTFLAG_ALIGNCENTER);
    self->board.next_piece_label = self->next_piece_label;

    self->score_lcd = q_lcdnumber_new3(5);
    q_lcdnumber_set_segment_style(self->score_lcd, QLCDNUMBER_SEGMENTSTYLE_FILLED);
    self->level_lcd = q_lcdnumber_new3(2);
    q_lcdnumber_set_segment_style(self->level_lcd, QLCDNUMBER_SEGMENTSTYLE_FILLED);
    self->lines_lcd = q_lcdnumber_new3(5);
    q_lcdnumber_set_segment_style(self->lines_lcd, QLCDNUMBER_SEGMENTSTYLE_FILLED);

    self->new_game_button = q_pushbutton_new3("&New Game");
    q_pushbutton_set_focus_policy(self->new_game_button, QT_FOCUSPOLICY_NOFOCUS);

    QKeySequence* new_key_sequence = q_keysequence_new2("Ctrl+N");
    QShortcut* new_shortcut = q_shortcut_new2(new_key_sequence, self->new_game_button);
    q_shortcut_on_activated(new_shortcut, on_new_game_activated);
    q_keysequence_delete(new_key_sequence);

    self->quit_button = q_pushbutton_new3("&Quit");
    q_pushbutton_set_focus_policy(self->quit_button, QT_FOCUSPOLICY_NOFOCUS);

    QKeySequence* quit_key_sequence = q_keysequence_new6(QKEYSEQUENCE_STANDARDKEY_QUIT);
    QShortcut* quit_shortcut = q_shortcut_new2(quit_key_sequence, self->quit_button);
    q_shortcut_on_activated(quit_shortcut, on_quit_activated);
    q_keysequence_delete(quit_key_sequence);

    self->pause_button = q_pushbutton_new3("&Pause");
    q_pushbutton_set_focus_policy(self->pause_button, QT_FOCUSPOLICY_NOFOCUS);
    q_pushbutton_set_disabled(self->pause_button, true);

    QKeySequence* pause_key_sequence = q_keysequence_new6(QKEYSEQUENCE_STANDARDKEY_CANCEL);
    QShortcut* pause_shortcut = q_shortcut_new2(pause_key_sequence, self->pause_button);
    q_shortcut_on_activated(pause_shortcut, on_pause_activated);
    q_keysequence_delete(pause_key_sequence);

    q_pushbutton_on_clicked(self->new_game_button, new_game);
    q_pushbutton_on_clicked(self->quit_button, quit_game);
    q_pushbutton_on_clicked(self->pause_button, pause_game);

    self->game_over_label = q_label_new2();
    q_label_set_text_format(self->game_over_label, QT_TEXTFORMAT_MARKDOWNTEXT);
    q_label_set_alignment(self->game_over_label, QT_ALIGNMENTFLAG_ALIGNCENTER);
    q_label_set_text(self->game_over_label, "# Game Over");

    QSizePolicy* label_policy = q_label_size_policy(self->game_over_label);
    q_sizepolicy_set_retain_size_when_hidden(label_policy, true);
    q_label_set_size_policy(self->game_over_label, label_policy);
    q_label_hide(self->game_over_label);
    q_sizepolicy_delete(label_policy);

    self->window = q_widget_new2();
    QGridLayout* layout = q_gridlayout_new(self->window);
    q_gridlayout_add_widget2(layout, create_label("NEXT"), 0, 0);
    q_gridlayout_add_widget2(layout, self->next_piece_label, 1, 0);
    q_gridlayout_add_widget2(layout, create_label("LEVEL"), 2, 0);
    q_gridlayout_add_widget2(layout, self->level_lcd, 3, 0);
    q_gridlayout_add_widget2(layout, self->new_game_button, 4, 0);
    q_gridlayout_add_widget2(layout, self->game_over_label, 5, 0);
    q_gridlayout_add_widget3(layout, self->board.frame, 0, 1, 6, 1);
    q_gridlayout_add_widget2(layout, create_label("SCORE"), 0, 2);
    q_gridlayout_add_widget2(layout, self->score_lcd, 1, 2);
    q_gridlayout_add_widget2(layout, create_label("LINES REMOVED"), 2, 2);
    q_gridlayout_add_widget2(layout, self->lines_lcd, 3, 2);
    q_gridlayout_add_widget2(layout, self->quit_button, 4, 2);
    q_gridlayout_add_widget2(layout, self->pause_button, 5, 2);
    q_gridlayout_set_column_stretch(layout, 0, 1);
    q_gridlayout_set_column_stretch(layout, 1, 2);
    q_gridlayout_set_column_stretch(layout, 2, 2);
    q_widget_set_layout(self->window, layout);

    q_widget_set_window_title(self->window, "Qt 6 Tetrix Example");
    q_widget_set_fixed_size2(self->window, 1000, 750);
}

void cleanup_window(const TetrixWindow* self) {
    cleanup_board(&self->board);
    q_widget_delete(self->window);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    initialize_tetrix_window(&tetrix_window);
    global_board = tetrix_window.board;

    q_widget_show(tetrix_window.window);

    QMessageBox* message_box = q_messagebox_new(tetrix_window.window);
    q_messagebox_set_option2(message_box, QMESSAGEBOX_OPTION_DONTUSENATIVEDIALOG, true);
    q_messagebox_set_window_modality(message_box, QT_WINDOWMODALITY_APPLICATIONMODAL);
    q_messagebox_set_text_format(message_box, QT_TEXTFORMAT_MARKDOWNTEXT);
    q_messagebox_set_window_title(message_box, "Game Controls");
    q_messagebox_set_text(message_box, "### - Left/Right: Move piece\n"
                                       "### - Down/Up: Rotate piece\n"
                                       "### - D: Move piece one line down\n"
                                       "### - Space: Drop piece\n"
                                       "### - Alt+N/Ctrl+N: New game\n"
                                       "### - Alt+Q/Ctrl+Q: Quit\n"
                                       "### - Alt+P/Esc: Pause");
    q_messagebox_show(message_box);

    int result = q_application_exec();

    cleanup_window(&tetrix_window);
    q_application_delete(qapp);

    return result;
}
