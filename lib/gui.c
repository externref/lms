#include <stdio.h>
#include "gtk/gtk.h"
#include "gtk/gtkshortcut.h"
#include "lib/database.c"


GtkWidget *GUI_search_results;
GtkWidget *GUI_search_sort;
GtkWidget *GUI_search_results_grid;
char search_sort_selection[7];
int grid_row = 0;


void __add_item_to_grid(struct Book book) {
    GtkWidget *label_name = gtk_label_new(book.name);
    GtkWidget *label_author = gtk_label_new(book.author);
    GtkWidget *label_category = gtk_label_new(book.category);
    
    char year_str[10];
    sprintf(year_str, "%d", book.year);
    GtkWidget *label_year = gtk_label_new(year_str);
    GtkWidget *label_borrowed = gtk_label_new(book.borrowed? "Unavailable": "Avaialble");
    gtk_grid_attach(GTK_GRID(GUI_search_results_grid), label_name, 0, grid_row, 1, 1);   // Attach name at column 0
    gtk_grid_attach(GTK_GRID(GUI_search_results_grid), label_author, 1, grid_row, 1, 1); // Attach author at column 1
    gtk_grid_attach(GTK_GRID(GUI_search_results_grid), label_category, 2, grid_row, 1, 1); // Attach category at column 2
    gtk_grid_attach(GTK_GRID(GUI_search_results_grid), label_year, 3, grid_row, 1, 1);   // Attach year at column 3
	gtk_grid_attach(GTK_GRID(GUI_search_results_grid), label_borrowed, 4, grid_row, 1, 1);
    gtk_widget_show(label_name);
    gtk_widget_show(label_author);
    gtk_widget_show(label_category);
    gtk_widget_show(label_year);
    gtk_widget_show(label_borrowed);
    grid_row++;  }



// Add grid headers
void __add_grid_header() {
    GtkWidget *header_name = gtk_label_new("Name");
    GtkWidget *header_author = gtk_label_new("Author");
    GtkWidget *header_category = gtk_label_new("Category");
    GtkWidget *header_year = gtk_label_new("Year");
    GtkWidget *header_borrowed = gtk_label_new("Borrow");
  
	GtkStyleContext *style_context;
	GtkCssProvider *style_provider;

     style_provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(
        style_provider, "* {padding: 0px; font-size: 16px; color: white; border-bottom: 1px solid white; padding-bottom: 5px; }", -1);

    style_context = gtk_widget_get_style_context(header_name);
  
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(style_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);    style_context = gtk_widget_get_style_context(GUI_search_results_grid);
 style_context = gtk_widget_get_style_context(header_author); 
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(style_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);    style_context = gtk_widget_get_style_context(GUI_search_results_grid);
  style_context = gtk_widget_get_style_context(header_category);
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(style_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);    style_context = gtk_widget_get_style_context(GUI_search_results_grid);
  style_context = gtk_widget_get_style_context(header_year);
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(style_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
style_context = gtk_widget_get_style_context(header_borrowed);    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(style_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
     gtk_widget_set_hexpand(header_name, TRUE);
    gtk_widget_set_hexpand(header_author, TRUE);
    gtk_widget_set_hexpand(header_category, TRUE);
    gtk_widget_set_hexpand(header_year, TRUE);
    gtk_widget_set_hexpand(header_borrowed, TRUE);

    // Attach headers at row 0
    gtk_grid_attach(GTK_GRID(GUI_search_results_grid), header_name, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(GUI_search_results_grid), header_author, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(GUI_search_results_grid), header_category, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(GUI_search_results_grid), header_year, 3, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(GUI_search_results_grid), header_borrowed, 4, 0, 1, 1);

    gtk_widget_show(header_name);
    gtk_widget_show(header_author);
    gtk_widget_show(header_category);
    gtk_widget_show(header_year);
    gtk_widget_show(header_borrowed);

    grid_row++;  
}

void __fresh_grid(){
       while (grid_row > 0) {
        gtk_grid_remove_row(GTK_GRID(GUI_search_results_grid), --grid_row);
    }

    grid_row = 0;  // Reset grid row

    // Add the header row again
    __add_grid_header();

       for(int i=0; i<total_books;i++){
	__add_item_to_grid(books[i]);
    }


}

// Callback for the search entry text
void on_item_search_entry(GtkSearchEntry *search_entry, gpointer user_data) {
    const char *search_text = gtk_editable_get_text(GTK_EDITABLE(search_entry));
    char dump[100];
    strcpy(dump, search_text);
   
    if (strlen(search_text) < 5) {
        gtk_label_set_text(GTK_LABEL(GUI_search_results),
                           "Enter atleast 5 letters to search.");
       
		__fresh_grid();
	return;
    }
    struct Book *books = d_get_books(dump, search_sort_selection);

    // Clear previous results
    while (grid_row > 0) {
        gtk_grid_remove_row(GTK_GRID(GUI_search_results_grid), --grid_row);
    }

    grid_row = 0;  // Reset grid row

    // Add the header row again
    __add_grid_header();

    int flag = 0;
    while (books[flag].name != NULL) {
        __add_item_to_grid(books[flag]);
        flag++;
    }
    free(books);

    char display_text[100] = "Showing results for: ";
    strcat(display_text, search_text);
    gtk_label_set_text(GTK_LABEL(GUI_search_results), display_text);
}

// Callback for sorting selection
void on_book_search_sort_by(GtkDropDown *dropdown, gpointer user_data) {
    gint index = gtk_drop_down_get_selected(dropdown);
    char values[3][7] = {"name", "author", "year"};
    strcpy(search_sort_selection, values[index]);
}

// Initialize the GUI
void gui_init_gui(GtkApplication *application) {
    sqlite3 *db;
    int rc = sqlite3_open("../library.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database file.'");
        return;
    }
    d_setups(db);

    GtkWidget *window = gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(window), "Library Management System");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkStyleContext *style_context;
    GtkCssProvider *style_provider;

    // Window element
    style_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(style_provider,
                                    "* { background-color: #131413; }", -1);
    style_context = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(style_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);

    // TOP LEVEL ELEMENTS
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // header
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    // Top text element
    GtkWidget *label = gtk_label_new("Library Management System");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    style_context = gtk_widget_get_style_context(label);
    style_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(
        style_provider, "* { font-size: 24px; color: white; padding: 10px; }",
        -1);
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(style_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_box_append(GTK_BOX(hbox), label);

    // header buttons styles
    style_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(style_provider,
                                    "* {background: #10a31d; margin: "
                                    "10px; color: #131413; border: 0px; }",
                                    -1);

    // spacer for indentation
    GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(spacer, TRUE);
    gtk_box_append(GTK_BOX(hbox), spacer);

    // "Manage Users" button
    GtkWidget *manage_users_button = gtk_button_new_with_label("Manage Users");
    gtk_box_append(GTK_BOX(hbox), manage_users_button);
    style_context = gtk_widget_get_style_context(manage_users_button);
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(style_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);

    // "Borrows" button
    GtkWidget *borrows_button = gtk_button_new_with_label("Borrows");
    gtk_box_append(GTK_BOX(hbox), borrows_button);
    style_context = gtk_widget_get_style_context(borrows_button);
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(style_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_box_append(GTK_BOX(box), hbox);

    // Separator line
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_append(GTK_BOX(box), sep);

    // Search box
    GtkWidget *search = gtk_search_entry_new();
    g_signal_connect(search, "search-changed", G_CALLBACK(on_item_search_entry),
                     NULL);

    // Style for search entry (keeping your original CSS intact)
    style_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(style_provider,
                                    "* { margin: 10px; color: black; }", -1);
    style_context = gtk_widget_get_style_context(search);
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(style_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_box_append(GTK_BOX(box), search);

    // results view
    GtkWidget *resbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    // search result text
    GUI_search_results = gtk_label_new("Enter atleast 5 letters to search.");
    style_provider = gtk_css_provider_new();
    style_context = gtk_widget_get_style_context(GUI_search_results);
    gtk_css_provider_load_from_data(style_provider,
                                    "* { margin-left: 10px; color: cyan;}", -1);
    gtk_label_set_xalign(GTK_LABEL(GUI_search_results), 0.0);
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(style_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_box_append(GTK_BOX(resbox), GUI_search_results);

    // space divider
    GtkWidget *spacer_2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(spacer_2, TRUE);
    gtk_box_append(GTK_BOX(resbox), spacer_2);

    // search sort

    GtkWidget *sort_by = gtk_label_new("Sort by: ");
    gtk_box_append(GTK_BOX(resbox), sort_by);
    style_provider = gtk_css_provider_new();
    style_context = gtk_widget_get_style_context(sort_by);
    gtk_css_provider_load_from_data(style_provider, "* { color: white; }", -1);
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(style_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);

    GtkStringList *sort_opts =
        gtk_string_list_new((const char *[]){"Name", "Author", "Year", NULL});
    GUI_search_sort = gtk_drop_down_new(G_LIST_MODEL(sort_opts), NULL);
    style_provider = gtk_css_provider_new();
    style_context = gtk_widget_get_style_context(GUI_search_sort);
    gtk_css_provider_load_from_data(
        style_provider, "* { margin: 10px; padding: 0px; font-size: 12px; }", -1);
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(style_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_box_append(GTK_BOX(resbox), GUI_search_sort);
    gtk_box_append(GTK_BOX(box), resbox);

    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,  // Horizontal scrolling policy
                                   GTK_POLICY_AUTOMATIC);
     gtk_widget_set_size_request(scrolled_window, 800, 400);

    // Initialize the grid and add styles
    GUI_search_results_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(GUI_search_results_grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(GUI_search_results_grid), 10);
    style_provider = gtk_css_provider_new();
    style_context = gtk_widget_get_style_context(GUI_search_results_grid);
    gtk_css_provider_load_from_data(
        style_provider, "* { margin: 10px; padding: 0px; font-size: 12px; color: #a39c9b; }", -1);
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(style_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), GUI_search_results_grid);

    // Add the scrolled window to the main box
    gtk_box_append(GTK_BOX(box), scrolled_window); 

    __add_grid_header();

    for(int i=0; i<total_books;i++){
	__add_item_to_grid(books[i]);
    }

    gtk_window_set_child(GTK_WINDOW(window), box);
    gtk_widget_show(window);
}

