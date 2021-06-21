/////////////////////////////////////////////////////////////////////
// PersistHelp.cpp - demo one way to persist objects to XML file   //
//                                                                 //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////

#include "PersistHelp.h"
#include <iostream>
#include "../Utilities/Utilities.h"
using namespace Utilities;
using namespace NoSQLDB;

using SPtr = std::shared_ptr<AbstractXmlElement>;

//----< saves instance state as XML string >-------------------------

WidgetWrapper::Xml WidgetWrapper::save()
{
  XmlDocument doc;

  // make and add root element to doc
  SPtr root = makeTaggedElement("WidgetWrapper");
  doc.docElement() = root;

  // make Property<int> wint element and add to root
  int wintValue = static_cast<Widget>(*this).wint;
  std::string wintStr = Utilities::Convert<int>::toString(wintValue);
  SPtr pWintElem = makeTaggedElement("wint");
  pWintElem->addChild(makeTextElement(wintStr));
  root->addChild(pWintElem);

  // make Property<std::string> wstr element and add to root
  std::string wStr = static_cast<Widget>(*this).wstr;
  SPtr pWstrElem = makeTaggedElement("wstr");
  pWstrElem->addChild(makeTextElement(wStr));
  root->addChild(pWstrElem);
  return doc.toString();
}
//---< restores instance state from XML string >---------------------

void WidgetWrapper::restore(const Xml& xml)
{
  XmlDocument doc(xml);
  std::vector<SPtr> elems = doc.element("wint").select();
  std::string temp = elems[0]->children()[0]->value();
  widget_.wint = Utilities::Convert<int>::fromString(temp);
  elems = doc.element("wstr").select();
  widget_.wstr = elems[0]->children()[0]->value();
}
//----< cast operator returns Widget member >------------------------

WidgetWrapper::operator Widget()
{
  return widget_;
}

#ifdef TEST_PERSISTHELP
/*
 * - You can define and undefine using 
 *   project > Properties > C/C++ > Preprocessor > Processor Definitions
 * - or using command line
 *   cl /DTEST_PERSISTHELP persist.cpp [plus all the other cpp files needed]
 */

int main()
{
  Title("Demonstrate Persisting to and from XML");
  putLine();

  subTitle("Persisting a string");
  StringWrapper sw("Hello World");
  sw = "hello world";     // using assignment overload
  std::string temp = sw;  // using implicit cast
  std::cout << "\n  StringWrapper state = " << static_cast<std::string>(sw);
  StringWrapper::Xml xml = sw.save();
  std::cout << "\n  XML saved from StringWrapper = " << xml;

  StringWrapper newSw;
  newSw.restore(xml);

  std::cout << "\n  id of restored StringWrapper = " << newSw.id();
  std::cout << "\n  restored StringWrapper state = " << static_cast<std::string>(newSw);
  putLine();

  subTitle("Persisting a double");
  DoubleWrapper dw(3.1415927);
  DoubleWrapper::Xml xmlD = dw.save();
  std::cout << "\n  DoubleWrapper state = " << static_cast<double>(dw);
  std::cout << "\n  XML saved from DoubleWrapper = " << xmlD;
  
  DoubleWrapper newDw;
  newDw.restore(xmlD);

  std::cout << "\n  id of restored DoubleWrapper = " << newDw.id();
  std::cout << "\n  restored DoubleWrapper state = " << static_cast<double>(newDw);
  putLine();

  subTitle("Persisting an Element<std::string>");
  Element<std::string> elem;
  elem.name = "testElemName";
  elem.category = "test";
  elem.dateTime = DateTime();
  elem.children.getValue().push_back("Key1");
  elem.children.getValue().push_back("Key2");
  elem.data = "myData";
  ElementWrapper<std::string> elemWrapper(elem);
  std::string elemXml = elemWrapper.save();
  std::cout << "\n  ElementWrapper<std::string> XML: " << elemXml << "\n\n";

  subTitle("Creating new Element<std::string> from persisted XML");
  Element<std::string> newElem;
  ElementWrapper<std::string> newElemWrapper(newElem);
  newElemWrapper.restore(elemXml);
  newElem = newElemWrapper;
  std::cout << newElem.show();
  Utilities::putLine();

  subTitle("Persisting a Widget");
  Widget wdgt;
  wdgt.wint = 42;
  wdgt.wstr = "Fred";
  WidgetWrapper wwdgt(wdgt);
  std::cout << "\n  WidgetWrapper state:";
  std::cout << "\n    wint = " << static_cast<Widget>(wwdgt).wint;
  std::cout << "\n    wstr = " << static_cast<Widget>(wwdgt).wstr;
  WidgetWrapper::Xml xmlW = wwdgt.save();
  std::cout << "\n  XML saved from WidgetWrapper:" << xmlW;

  WidgetWrapper newWwdgt;
  newWwdgt.restore(xmlW);
  std::cout << "\n  id of restored WidgetWrapper = " << newWwdgt.id();
  std::cout << "\n  WidgetWrapper state:";
  std::cout << "\n    wint = " << static_cast<Widget>(newWwdgt).wint;
  std::cout << "\n    wstr = " << static_cast<Widget>(newWwdgt).wstr;
  putLines(2);
}

#endif
