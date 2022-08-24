#pragma once
#include <string>
#define MAX_PRIMITIVETEXT_SIZE 16
struct PrimitiveText
{
  PrimitiveText() = default;
  PrimitiveText(std::wstring text)
  {
    this->text = text;
  }
  void AddChar(wchar_t letter)
  {
    if (text.size() < MAX_PRIMITIVETEXT_SIZE)
      text += letter;
  }

  void DeleteChar()
  {
    if (!text.empty())
      text.erase(text.size() - 1);
  }

  void Update(std::wstring text)
  {
    this->text = text;
  }
  const std::wstring& GetString() const
  {
    return text;
  }
  void Clear()
  {
    text.clear();
  }
private:
  std::wstring text;
};