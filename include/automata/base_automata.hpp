#pragma once

#include <string>

class BaseAutomata {
 public:
  virtual bool allows(const std::string&) const = 0;
  virtual void visualize(std::ostream&) const = 0;
  virtual size_t size() const = 0;
};