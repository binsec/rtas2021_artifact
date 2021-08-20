#!/usr/bin/python

from xml.dom import minidom

text_test = """
AC_CHECK_HEADERS([inttypes.h],,\\
	AC_MSG_ERROR([The 'inttypes.h' header is missing.]))	
"""

def getText(nodelist):
    rc = ""
    for node in nodelist:
        if node.nodeType == node.TEXT_NODE:
            rc = rc + node.data
    return rc

impl = minidom.getDOMImplementation()

newdoc = impl.createDocument(None, "test", None)
top_element = newdoc.documentElement
text = newdoc.createTextNode(text_test)
top_element.appendChild(text)

print newdoc.toprettyxml(encoding="utf-8")
node = newdoc.getElementsByTagName("test")[0]
print "%s" % getText(node.childNodes)

