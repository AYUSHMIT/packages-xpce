
/* xcolour.c */
status		ws_create_colour(Colour c, DisplayObj d);
status		ws_colour_name(DisplayObj d, Name name);
Colour		ws_pixel_to_colour(DisplayObj d, ulong pixel);

/* xcursor.c */
void		ws_init_cursor_font(void);
Int		ws_cursor_font_index(Name name);
status		ws_create_cursor(CursorObj c, DisplayObj d);
void		ws_destroy_cursor(CursorObj c, DisplayObj d);

/* xdisplay.c */
void		ws_flush_display(DisplayObj d);
void		ws_synchronise_display(DisplayObj d);
void		ws_bell_display(DisplayObj d, int volume);
void		ws_get_size_display(DisplayObj d, int *w, int *h);
int		ws_depth_display(DisplayObj d);
void		ws_activate_screen_saver(DisplayObj d);
void		ws_deactivate_screen_saver(DisplayObj d);
void		ws_init_display(DisplayObj d);
status		ws_legal_display_name(char *s);
status		ws_opened_display(DisplayObj d);
void		ws_open_display(DisplayObj d);
status		ws_init_graphics_display(DisplayObj d);
void		ws_foreground_display(DisplayObj d, Colour c);
void		ws_background_display(DisplayObj d, Colour c);
void		ws_draw_in_display(DisplayObj d, Graphical gr, Bool invert, Bool subtoo);
void		ws_grab_server(DisplayObj d);
void		ws_ungrab_server(DisplayObj d);
Int		ws_display_connection_number(DisplayObj d);
status		ws_events_queued_display(DisplayObj d);
status		ws_set_cutbuffer(DisplayObj d, int n, String s);
StringObj	ws_get_cutbuffer(DisplayObj d, int n);
ulong		ws_get_selection_timeout(void);
void		ws_set_selection_timeout(ulong time);
Any		ws_get_selection(DisplayObj d, Name which, Name target);
void		ws_disown_selection(DisplayObj d, Name selection);
status		ws_own_selection(DisplayObj d, Name selection);
Name		ws_window_manager(DisplayObj d);
void		ws_synchronous(DisplayObj d);
void		ws_asynchronous(DisplayObj d);
status		ws_postscript_display(DisplayObj d);
StringObj	ws_get_resource_value(DisplayObj d, Name cc, Name cn, Name rc, Name rn);

/* xdraw.c */
void		resetDraw(void);
void		d_offset(int x, int y);
DisplayObj	d_display(DisplayObj d);
void		d_flush(void);
void		d_window(PceWindow sw, int x, int y, int w, int h, int clear, int limit);
void		d_image(Image i, int x, int y, int w, int h);
void		d_screen(DisplayObj d);
void		d_clip(int x, int y, int w, int h);
void		d_done(void);
void		d_clip_done(void);
void		intersection_iarea(IArea a, IArea b);
void		r_clear(int x, int y, int w, int h);
void		r_complement(int x, int y, int w, int h);
void		r_and(int x, int y, int w, int h, Image pattern);
void		r_thickness(int pen);
void		r_dash(Name name);
void		r_fillpattern(Any fill);
void		r_arcmode(Name mode);
Colour		r_default_colour(Colour c);
Colour		r_colour(Colour c);
void		r_swap_background_and_foreground(void);
void		r_subwindow_mode(Bool val);
void		r_invert_mode(Bool val);
void		r_translate(int x, int y, int *ox, int *oy);
void		r_box(int x, int y, int w, int h, int r, Image fill);
void		r_shadow_box(int x, int y, int w, int h, int r, int shadow, Image fill);
void		r_3d_box(int x, int y, int w, int h, int shadow, Any fill, int up);
void		r_arc(int x, int y, int w, int h, int s, int e, Image fill);
void		r_ellipse(int x, int y, int w, int h, Image fill);
void		r_line(int x1, int y1, int x2, int y2);
void		r_polygon(IPoint pts, int n, int close);
void		r_path(Chain points, int ox, int oy, int radius, int closed, Image fill);
void		r_op_image(Image image, int sx, int sy, int x, int y, int w, int h, Name op);
void		r_image(Image image, int sx, int sy, int x, int y, int w, int h);
void		r_fill(int x, int y, int w, int h, Image pattern);
void		r_fill_polygon(IPoint pts, int n);
void		r_caret(int cx, int cy, FontObj font);
void		r_fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3);
void		r_triangle(int x1, int y1, int x2, int y2, int x3, int y3);
void		r_pixel(int x, int y, Any val);
void		r_complement_pixel(int x, int y);
void		d_modify(void);
int		r_get_mono_pixel(int x, int y);
unsigned long	r_get_pixel(int x, int y);
int		s_has_char(FontObj f, unsigned int c);
void		f_domain(FontObj f, Name which, int *x, int *y);
int		s_default_char(FontObj font);
int		s_ascent(FontObj f);
int		s_descent(FontObj f);
int		s_height(FontObj f);
int		c_width(unsigned int c, FontObj font);
String		str_bits_as_font(String s, FontObj f, int *shift);
int		str_width(String s, int from, int to, FontObj f);
void		s_print8(char8 *s, int l, int x, int y, FontObj f);
void		s_print16(char16 *s, int l, int x, int y, FontObj f);
void		str_size(String s, FontObj font, int *width, int *height);
void		str_string(String s, FontObj font, int x, int y, int w, int h, Name hadjust, Name vadjust);
void		ps_string(String s, FontObj font, int x, int y, int w, Name format);
void		str_label(char8 *s, char8 acc, FontObj font, int x, int y, int w, int h, Name hadjust, Name vadjust);

/* xevent.c */
status		ws_dispatch(Int FD, Int timeout);
Any		ws_event_in_subwindow(EventObj ev, Any root);

/* xfont.c */
status		ws_create_font(FontObj f, DisplayObj d);
void		ws_destroy_font(FontObj f, DisplayObj d);

/* xframe.c */
status		ws_created_frame(FrameObj fr);
void		ws_uncreate_frame(FrameObj fr);
status		ws_create_frame(FrameObj fr);
void		ws_realise_frame(FrameObj fr);
void		ws_show_frame(FrameObj fr, Bool grab);
void		ws_unshow_frame(FrameObj fr);
void		ws_raise_frame(FrameObj fr);
void		ws_lower_frame(FrameObj fr);
status		ws_attach_wm_prototols_frame(FrameObj fr);
status		ws_frame_bb(FrameObj fr, int *x, int *y, int *w, int *h);
void		ws_x_geometry_frame(FrameObj fr, Name spec);
void		ws_geometry_frame(FrameObj fr, Int x, Int y, Int w, Int h);
void		ws_border_frame(FrameObj fr, int b);
void		ws_busy_cursor_frame(FrameObj fr, CursorObj c);
void		ws_set_icon_frame(FrameObj fr);
void		ws_set_icon_label_frame(FrameObj fr);
void		ws_set_icon_position_frame(FrameObj fr, int x, int y);
status		ws_get_icon_position_frame(FrameObj fr, int *x, int *y);
void		ws_iconify_frame(FrameObj fr);
void		ws_deiconify_frame(FrameObj fr);
void		ws_set_label_frame(FrameObj fr);
Image		ws_image_of_frame(FrameObj fr);
void		ws_transient_frame(FrameObj fr, FrameObj fr2);
status		ws_postscript_frame(FrameObj fr);

/* ximage.c */
void		ws_init_image(Image image);
void		ws_destroy_image(Image image);
status		ws_store_image(Image image, FileObj file);
status		loadXImage(Image image, FILE *fd);
status		ws_load_old_image(Image image, FILE *fd);
status		ws_load_image_file(Image image);
status		ws_save_image_file(Image image, FileObj file);
status		ws_open_image(Image image, DisplayObj d);
void		ws_close_image(Image image, DisplayObj d);
status		ws_resize_image(Image image, Int w, Int h);
void		ws_postscript_image(Image image);
void		ws_create_image_from_x11_data(Image image, unsigned char *data, int w, int h);

/* xstream.c */
void		ws_close_stream(Stream s);
void		ws_input_stream(Stream s);
void		ws_listen_socket(Socket s);

/* xtimer.c */
void		ws_status_timer(Timer tm, Name status);

/* xwindow.c */
status		ws_created_window(PceWindow sw);
void		ws_uncreate_window(PceWindow sw);
status		ws_create_window(PceWindow sw, PceWindow parent);
void		ws_manage_window(PceWindow sw);
void		ws_unmanage_window(PceWindow sw);
void		ws_reassociate_ws_window(PceWindow from, PceWindow to);
void		ws_geometry_window(PceWindow sw, int x, int y, int w, int h, int pen);
void		ws_grab_pointer_window(PceWindow sw, Bool val);
void		ws_grab_keyboard_window(PceWindow sw, Bool val);
void		ws_grab_pointer_window(PceWindow sw, Bool val);
void		ws_grab_keyboard_window(PceWindow sw, Bool val);
void		ws_ungrab_all(void);
void		ws_move_pointer(PceWindow sw, int x, int y);
void		ws_window_cursor(PceWindow sw, CursorObj cursor);
void		ws_window_background(PceWindow sw, Any c);
void		ws_raise_window(PceWindow sw);
void		ws_lower_window(PceWindow sw);

/* x11.c */
int		ws_version(void);
int		ws_revision(void);
status		ws_expose_console(void);
status		ws_console_label(CharArray label);
