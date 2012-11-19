#include "ggmain.hpp"

int main (int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);

  Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();
  builder->add_from_file("src/window.2.glade");

  Annotation * window;

  builder->get_widget_derived("window1", window); 

  Gtk::Main::run(*window);
}
