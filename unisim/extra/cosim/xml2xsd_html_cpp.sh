#!/bin/sh

# echo Usage: xml2xsd_html_cpp.sh <xml_base_file> <xsd_name> <root_element_name>

# trang -I xml -O xsd Test_Case1.xml Test_Case.xsd

xsd cxx-parser --root-element testCase Test_Case.xsd
xsd cxx-tree --root-element testCase Test_Case.xsd

xsltproc --nonet --output Test_Case.html xs3p.xsl Test_Case.xsd

