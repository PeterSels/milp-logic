#ifndef MY_COLORS_H
#define MY_COLORS_H

#include <string>
#include <map>

class Rgb {
public:
  Rgb();
  Rgb(unsigned int r, unsigned int g, unsigned int b);
  std::string toString(const std::string format) const;
  ~Rgb();
private:
  unsigned int r_;
  unsigned int g_;
  unsigned int b_;
};

class MyColors {
public:
  MyColors();
  void addColor(const std::string colorName, const Rgb rgb);
  const Rgb & getColor(const std::string colorName) const;
  std::string getColor(const std::string colorName,
                       const std::string format) const;
  ~MyColors();
private:
  std::map<std::string, Rgb> colors_;
};

#endif // MY_COLORS_H
