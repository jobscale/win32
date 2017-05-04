# -*- coding: utf-8 -*-

from __future__ import with_statement
import base64
import codecs
import os
import re
import tempfile

from mako.template import Template

BASE_PATH = os.path.dirname(os.path.abspath(__file__))

SRC_DIR =  BASE_PATH + '/../src/pLook/res'
DST_DIR =  BASE_PATH + '/../src/pLook'
TPL_DIR =  BASE_PATH + '/../src/pLook'

TPL_OPTS = {
  'filename': TPL_DIR+'/pConsolePage.inl.mako',
  'input_encoding': 'cp932',
  'output_encoding': 'cp932',
}
CMP_ARGS = [
  'java',
  '-jar',
  './yuicompressor/yuicompressor.jar',
  '--line-break',
  '80',
  '--charset',
  'utf-8',
  '-o',
]
COPY_RIGHT = (
'"/*\\n"\r\n'
'    " * %s\\n"\r\n'
'    " * (c) 2006-2012 Plustar Co. Ltd.\\n"\r\n'
'    " * All rights reserved.\\n"\r\n'
'    " */\\n"\r\n'
'    "\\n"\r\n'
)


class Converter(object):
  def __init__(self, fname):
    self.path = "%s/%s" % (SRC_DIR, fname)

  def read(self):
    with file(self.path, 'r') as f:
      return codecs.getreader('utf-8')(f).read()

  def conv(self, s):
    return s

  def perform(self):
    return self.conv(self.read())


class Compressor(Converter):
  def read(self):
    tmp = tempfile.NamedTemporaryFile()
    args = CMP_ARGS[:]
    args.append(tmp.name)
    args.append(self.path)

    if os.fork() == 0:
      os.execvp('java', args)
    os.wait()

    self.path = tmp.name
    return Converter.read(self)


class HtmlConverter(Converter):
  def conv(self, s):
    s = re.sub(r'<\\/a><\\/p>', r'<\\\\/a><\\\\/p>', s)
    s = re.sub(r'"', r'\"', s)
    s = re.sub(r'\n$', r'\\n"', s)
    s = re.sub(r'\n', r'\\n"\r\n    "', s)
    return '"' + s


class CssConverter(Compressor):
  def conv(self, s):
    s = re.sub(r'"', r'\"', s)
    s = re.sub(r'\n$', r'"', s)
    s = re.sub(r'\n', r'"\r\n    "', s)
    cr = COPY_RIGHT % 'extrapper.css - Stylesheet for ExTrapper'
    return '%s    "%s\\n"' % (cr, s)


class JsConverter(Compressor):
  def conv(self, s):
    s = re.sub(r'\\', r'\\\\', s)
    s = re.sub(r'"', r'\"', s)
    s = re.sub(r'\n$', r'"', s)
    s = re.sub(r'\n', r'"\r\n    "', s)
    cr = COPY_RIGHT % 'extrapper.js - JavaScript for ExTrapper'
    return '%s    "%s\\n"' % (cr, s)


class ImageConverter(Converter):
  def read(self):
    with file(self.path, 'rb') as f:
      return base64.encodestring(f.read())

  def conv(self, s):
    s = re.sub(r'\n$', r'"', s)
    s = re.sub(r'\n', r'"\r\n    "', s)
    return '"' + s


index_html = HtmlConverter('index.html').perform()
index_html3 = HtmlConverter('index3.html').perform()
err_401_html = HtmlConverter('401.html').perform()
err_404_html = HtmlConverter('404.html').perform()
err_500_html = HtmlConverter('500.html').perform()
exp_lic_html = HtmlConverter('exp_lic.html').perform()
extrapper_css = CssConverter('extrapper.css').perform()
extrapper_css3 = CssConverter('extrapper3.css').perform()
extrapper_js = JsConverter('extrapper.js').perform()
extrapper_js3 = JsConverter('extrapper3.js').perform()
logo_png = ImageConverter('logo.png').perform()
logo_png_platinum = ImageConverter('logo-Platinum.png').perform()
logo_png_gold = ImageConverter('logo-Gold.png').perform()
logo_png_silver = ImageConverter('logo-Silver.png').perform()
logo_png_trial = ImageConverter('logo-Trial.png').perform()
powered_png = ImageConverter('powered.png').perform()

with file(DST_DIR+'/pConsolePage.inl', 'w') as f:
  f.write(Template(**TPL_OPTS).render(**vars()))

os.utime(DST_DIR+'/pReceiveBCast.cpp', None)
