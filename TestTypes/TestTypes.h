#pragma once
// TestTypes.h

#include "../CppProperties/CppProperties.h"
#include "../Persist/PersistHelp.h"
#include "../Utilities/Utilities.h"
#include <iostream>

namespace NoSQLDB
{
  class SuperHero
  {
  public:
    Property<std::string> name;
    Property<double> power;
  };

  inline std::ostream& operator<<(std::ostream& out, SuperHero& sh)
  {
    out << static_cast<std::string>(sh.name).c_str() << " : " << static_cast<double>(sh.power);
    return out;
  }

  class SuperHeroWrapper : public IPersist<SuperHero>
  {
  public:
    SuperHeroWrapper() {};
    SuperHeroWrapper(SuperHero superHero) : superHero_(superHero) {}
    SuperHeroWrapper& operator=(const SuperHero& superHero);
    Xml save();
    void restore(const Xml& xml);
    operator SuperHero();
  private:
    SuperHero superHero_;
  };
  //----< assignment operator overload >-------------------------------

  inline SuperHeroWrapper& SuperHeroWrapper::operator=(const SuperHero& superHero)
  {
    superHero_ = superHero;
    return *this;
  }

  using SPtr = std::shared_ptr<AbstractXmlElement>;

  //----< saves instance state as XML string >-------------------------

  inline SuperHeroWrapper::Xml SuperHeroWrapper::save()
  {
    XmlDocument doc;

    // make and add root element to doc
    SPtr root = makeTaggedElement("SuperHeroWrapper");
    doc.docElement() = root;

    // make Property<double> power element and add to root
    double powerValue = (static_cast<SuperHero>(*this)).power;
    std::string powerStr = Utilities::Convert<double>::toString(powerValue);
    SPtr pPowerElem = makeTaggedElement("power");
    pPowerElem->addChild(makeTextElement(powerStr));
    root->addChild(pPowerElem);

    // make Property<std::string> name element and add to root
    std::string nameStr = static_cast<SuperHero>(*this).name;
    SPtr pNameElem = makeTaggedElement("name");
    pNameElem->addChild(makeTextElement(nameStr));
    root->addChild(pNameElem);
    return doc.toString();
  }
  //---< restores instance state from XML string >---------------------

  inline void SuperHeroWrapper::restore(const Xml& xml)
  {
    XmlDocument doc(xml);
    std::vector<SPtr> elems = doc.element("power").select();
    std::string temp = elems[0]->children()[0]->value();
    superHero_.power = Utilities::Convert<double>::fromString(temp);
    elems = doc.element("name").select();
    superHero_.name = Utilities::trim(elems[0]->children()[0]->value());
  }
  //----< cast operator returns SuperHero member >---------------------

  inline SuperHeroWrapper::operator SuperHero()
  {
    return superHero_;
  }
}