#!/usr/bin/env python2

from lib import SciOCR
import pygtk
pygtk.require('2.0')
import gtk

protoNet = "/home/fedor/Project/SciOCR/data/net/ruphy-28000.model.proto"
lstm = SciOCR.Clstm()
lstm.load(protoNet)
cvImg = SciOCR.Mat()
cvProcessed = SciOCR.Mat()
pixbuf = gtk.gdk.pixbuf_new_from_file("./data/dummy.bmp")
vec = SciOCR.vect_of_rect()
textBuf = gtk.TextBuffer()
ctext = ""

def browse_for(type):
	"""This function is used to browse for an image.
	The path to the image will be returned if the user
	selects one, however a blank string will be returned
	if they cancel or do not select one."""
	if( type == "image" ):
		title="Select Image"
	elif( type == "clstm_net" ):
		title="Select lstm net file"
	else:
		title="Something wrong!"

	file_open = gtk.FileChooserDialog(title
				, action=gtk.FILE_CHOOSER_ACTION_OPEN
				, buttons=(gtk.STOCK_CANCEL
							, gtk.RESPONSE_CANCEL
							, gtk.STOCK_OPEN
							, gtk.RESPONSE_OK))
	"""Create and add the Images filter"""
	filter = gtk.FileFilter()
	if( type == "image" ):
		filter.set_name("Images")
		filter.add_mime_type("image/png")
		filter.add_mime_type("image/jpeg")
		filter.add_mime_type("image/bmp")
		filter.add_pattern("*.png")
		filter.add_pattern("*.jpg")
		filter.add_pattern("*.bmp")
		file_open.add_filter(filter)
	elif( type == "clstm_net" ):
		filter.set_name("Net")
		filter.add_mime_type("net/lstm")
		filter.add_pattern("*.proto")
	"""Create and add the 'all files' filter"""
	filter = gtk.FileFilter()
	filter.set_name("All files")
	filter.add_pattern("*")
	file_open.add_filter(filter)

	"""Init the return value"""
	result = ""
	if file_open.run() == gtk.RESPONSE_OK:
		result = file_open.get_filename()
	file_open.destroy()

	return result

def destroy_window(self, window):
	window.destroy()
	return True

def save(self, window):
	global cvImg
	global cvProcessed
	global ctext
	global textBuf
	global vec
	
	
	if( ctext != "" ): 	
		start = textBuf.get_start_iter()
		end = textBuf.get_end_iter()
		ctext = textBuf.get_text(start, end, True)
		SciOCR.SaveAll(cvImg, cvProcessed, vec, ctext)
		#~ cvProcessed.save()
		destroy_window(self, window)
		
		

def create_window():
	window = gtk.Window(gtk.WINDOW_TOPLEVEL)
	window.set_border_width(5)
	
	global pixbuf
	global ctext
	global textBuf
	
	img = gtk.Image()
	img.set_from_pixbuf(pixbuf)
	
	vbox = gtk.VBox()
	vbox.set_spacing(5)
	
	imgFrame = gtk.Frame()
	textFrame = gtk.Frame()
	
	vbox.pack_start (imgFrame)
	vbox.pack_start (textFrame)
	
	imgFrame.add(img)
	
	textbox = gtk.TextView()
	textBuf.set_text(ctext)
	textbox.set_buffer(textBuf)
	textFrame.add(textbox)
	
	window.add(vbox)
	
	hbox = gtk.HBox()
	vbox.pack_start(hbox)
	
	saveBtn = gtk.Button()
	closeBtn = gtk.Button()
	saveBtn.set_label("Save")
	closeBtn.set_label("Close")
	
	hbox.pack_start(saveBtn,True, True, 0)
	hbox.pack_start(closeBtn,True, True, 0)
	
	saveBtn.connect("clicked", save, window)
	closeBtn.connect("clicked", destroy_window, window)
	
	window.show_all()

def close_app(data=None):
	gtk.main_quit()
	
def open_app(data=None):
	create_window()
	
def load_image(data=None):
	global cvImg
	global pixbuf
	path = browse_for("image")
	SciOCR.loadImg(path, cvImg)
	
	pixbuf = gtk.gdk.pixbuf_new_from_file(path)
	create_window()
	
def make_menu(event_button, event_time, data=None):
	menu = gtk.Menu()
	open_item = gtk.MenuItem("Show")
	load_img = gtk.MenuItem("Load image")
	test = gtk.MenuItem("Test run")
	close_item = gtk.MenuItem("Quit")

	#Append the menu items  
	menu.append(open_item)
	menu.append(load_img)
	menu.append(test)
	menu.append(close_item)
	
	#add callbacks
	open_item.connect_object("activate", open_app, "Open App")
	load_img.connect_object("activate", load_image, "Browse images")
	test.connect_object("activate", recognise, "Test run")
	close_item.connect_object("activate", close_app, "Quit")
	
	#Show the menu items
	menu.show_all()
	
	#Popup the menu
	menu.popup(None, None, None, event_button, event_time)

def on_left_click(event):
	gtk.gdk.pointer_ungrab(0L)
	global cvImg
	global pixbuf
	
	suc = SciOCR.getScreen(cvImg)
	if( suc == 1 ):
		exit(1)
	step = cvImg.cols*3
	pixbuf = gtk.gdk.pixbuf_new_from_data(cvImg.data(), gtk.gdk.COLORSPACE_RGB, False, 8, cvImg.cols, cvImg.rows, step)
	recognise('activate')
	create_window()

def on_right_click(data, event_button, event_time):
	make_menu(event_button, event_time)

def recognise(data=None):
	global cvImg
	global cvProcessed
	global lstm
	global vec
	global ctext

	SciOCR.preprocessing(cvImg, cvProcessed)
	SciOCR.shredder(vec, cvProcessed)
	
	ctext = SciOCR.ocr(vec, cvProcessed, lstm)
	
	print "Done"

def main():
	gtk.main()
	return 0

if __name__ == '__main__':
  icon = gtk.status_icon_new_from_stock(gtk.STOCK_ABOUT)
  icon.connect('popup-menu', on_right_click)
  icon.connect('activate', on_left_click)
  main()
  
  
  