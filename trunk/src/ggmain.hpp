#ifndef ANNOTATIONWINDOW_H
#define ANNOTATIONWINDOW_H

#include <gtkmm.h>
#include <iostream>


class Annotation : public Gtk::Window
{

public:
  Annotation(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
  virtual ~Annotation();

protected:
  Glib::RefPtr<Gtk::Builder> builder;
  void on_button1_clicked();
  void on_button2_clicked();

  //Member widgets:
  Gtk::Button * button1;
  Gtk::Button * button2;
};


Annotation::Annotation(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade) :
	Gtk::Window(cobject), builder(refGlade)
{
  // load data from glade xml
  builder->get_widget("button1", button1);
  builder->get_widget("button2", button2);

  // When the button receives the "clicked" signal, it will call the
  button1->signal_clicked().connect(sigc::mem_fun(*this,
              &Annotation::on_button1_clicked));

  button2->signal_clicked().connect(sigc::mem_fun(*this,
              &Annotation::on_button2_clicked));
}

Annotation::~Annotation()
{
}

void Annotation::on_button1_clicked()
{
  std::cout << "rec" << std::endl;
}

void Annotation::on_button2_clicked()
{
  std::cout << "stop" << std::endl;
}



#endif // ANNOTATIONWINDOW_H
