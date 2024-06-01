#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <ranges>

// boolean on the position of the current character
struct Inside {
  bool ref = false;
  bool comment = false;
  bool pub_type = false;
  bool entry_name = false;
  bool field_key = false;
  bool field_value = false;
};

// field of a reference
struct Field {
  std::string key;
  std::string value;
  signed char open_parentheses = 0;
  signed char skipped_parentheses = 0;
};

// reference
struct Ref {
  std::string comment;
  std::string pub_type;
  std::string entry_name;
  std::unordered_map<std::string, std::string> fields;
};
struct Reference {
  std::string comment;
  std::string pub_type;
  std::unordered_map<std::string, std::string> fields;
};


// function called in case of incorrectly specified references
void ref_error(Ref& ref, Field& field, Inside& inside) {
  if(ref.entry_name.empty()) {
    ref.entry_name = "unnamed entry";
  }
  std::cout << ref.entry_name << " is not correctly specified. Discarding entry." << std::endl;
  ref = Ref();
  field = Field();
  inside = Inside();
}


int main(int argc, char* argv[]) {
  const std::string in_path = argv[1];
  const std::string out_path = argv[2];

  // select fieldnames
  const std::vector<std::string> s_fieldnames_vector {"author", "title", "editor", "booktitle", "journal", "volume", "number", "year", "type", "pages",
    "publisher", "institution", "note"};

  // all references
  std::map<std::string, Reference> refs;

  // read file
  {
    std::ifstream fin(in_path);
    if(!fin.good()) {
      std::cout << in_path << " not readable." << std::endl;
      return 0;
    }
    std::string line;

    // select fieldnames
    const std::unordered_set<std::string> s_fieldnames_uset(s_fieldnames_vector.begin(), s_fieldnames_vector.end());
    const std::unordered_set<std::string> no_ws_fields {"volume", "number", "year", "pages"};

    // structs
    Inside inside;
    Field field;
    Ref ref;

    // loop over lines of references file
    while(std::getline(fin, line)) {
      if(!line.empty()) {
        inside.comment = false;
        for(const char& c : line) {
          // field value
          if(inside.field_value) {
            if(c == '{') {
              ++field.open_parentheses;
              if(field.value.empty()) {
                ++field.skipped_parentheses;
              } else {
                field.value += c;
              }
            } else if(c == '}') {
              --field.open_parentheses;
              if(field.open_parentheses < 0) {
                ref_error(ref, field, inside);
              } else if(field.open_parentheses == 0) {
                inside.field_value = false;
                inside.field_key = true;
                if(s_fieldnames_uset.contains(field.key)) {
                  // trim back white space
                  if(field.value.back() == ' ') {
                    field.value.pop_back();
                  }
                  // remove intial "the" from journal names
                  if(field.key == "journal" && field.value.size() > 4) {
                    std::string journal_start = field.value.substr(0, 4);
                    for(unsigned char i = 0; i < 4; ++i) {
                      journal_start[i] = std::tolower(journal_start[i]);
                    }
                    if(journal_start == "the ") {
                      field.value.erase(0, 4);
                    }
                  }
                  ref.fields[field.key] = field.value;
                }
                field = Field();
              } else if(field.skipped_parentheses <= field.open_parentheses) {
                field.value += c;
              }
            } else if(field.open_parentheses != 0) {
              if(field.value.empty()) {
                // trim front white space
                if(c != ' ') {
                  field.value += c;
                }
              } else if(c == ' ') {
                // omit white space in certain fields
                if(!no_ws_fields.contains(field.key)) {
                  // avoid double white space
                  if(field.value.back() != ' ') {
                    field.value += c;
                  }
                }
              // avoid double hyphen in pages field
              } else if(!(c == '-' && field.key == "pages" && field.value.back() == '-')) {
                field.value += c;
              }
            }
          // comment
          } else if(inside.comment) {
            if(!(c == ' ' && ref.comment.back() == ' ')) {
              ref.comment += c;
            }
          } else if(c == '%' && !inside.ref) {
            inside.comment = true;
            if(!ref.comment.empty()) {
              ref.comment += '\n';
            }
            ref.comment += "% ";
          // new entry
          } else if(c == '@') {
            if(inside.ref) {
              ref_error(ref, field, inside);
            }
            inside.ref = true;
            inside.pub_type = true;
          // publication type
          } else if(inside.pub_type) {
            if(c != ' ') {
              if(c == '{') {
                inside.pub_type = false;
                inside.entry_name = true;
              } else {
                ref.pub_type += std::tolower(c);
              }
            }
          // entry name
          } else if(inside.entry_name) {
            if(c == ',') {
              if(ref.entry_name.empty()) {
                ref_error(ref, field, inside);
              } else {
                ref.entry_name[0] = std::toupper(ref.entry_name[0]);
                inside.entry_name = false;
                inside.field_key = true;
              }
            } else if(c != ' ') {
              ref.entry_name += c;
            }
          // field key
          } else if(inside.field_key) {
            if(c == '}') {
              inside = Inside();
              field = Field();
              if(ref.fields.empty()) {
                std::cout << ref.entry_name << " without correctly specified fields. Discarding entry." << std::endl;
              } else {
                Reference reference {ref.comment, ref.pub_type, ref.fields};
                refs[ref.entry_name] = reference;
              }
              ref = Ref();
            } else if(c != ' ' && c != ',') {
              if(c == '=') {
                if(field.key.empty()) {
                  ref_error(ref, field, inside);
                } else {
                  inside.field_key = false;
                  inside.field_value = true;
                }
              } else {
                field.key += std::tolower(c);
              }
            }
          }
        }
      }
    }

    fin.close();
  }

  // write file
  std::ofstream fout(out_path);
  if(!fout.good()) {
    std::cout << out_path << " not writable." << std::endl;
    return 0;
  }
  if(refs.empty()) {
    std::cout << "No correctly specified references listed." << std::endl;
  } else {
    // file header listing number of references and names
    fout << "% " << refs.size() << " references\n% entry names: ";
    bool first_key {true};
    for(const std::string& key : std::views::keys(refs)) {
      if(first_key) {
        fout << key;
        first_key = false;
      } else {
        fout << ", " << key;
      }
    }
    fout << "\n\n";
    const std::unordered_set<std::string> titles {"title", "booktitle"};
    // white space between key and value by key
    const unsigned char s_fieldnames_vector_size = s_fieldnames_vector.size();
    std::vector<unsigned char> s_fieldnames_nchar(s_fieldnames_vector_size);
    unsigned char max_nchar {0};
    for(unsigned char i = 0; i < s_fieldnames_vector_size; ++i) {
      const unsigned char s_fieldnames_vector_i_size = s_fieldnames_vector[i].size();
      if(s_fieldnames_vector_i_size > max_nchar) {
        max_nchar = s_fieldnames_vector_i_size;
      }
      s_fieldnames_nchar[i] = s_fieldnames_vector_i_size;
    }
    ++max_nchar;
    for(unsigned char i = 0; i < s_fieldnames_vector_size; ++i) {
      s_fieldnames_nchar[i] = max_nchar - s_fieldnames_nchar[i];
    }
    // write references
    for(auto const& [key, value] : refs) {
      if(!value.comment.empty()) {
        fout << value.comment << '\n';
      }
      fout << '@' << value.pub_type << '{' << key;
      for(unsigned char i = 0; i < s_fieldnames_vector_size; ++i) {
        if(value.fields.contains(s_fieldnames_vector[i])) {
          fout << ",\n" << "  " << s_fieldnames_vector[i] << std::string(s_fieldnames_nchar[i], ' ') << "= {";
          // titles with dual parentheses
          const bool is_title = titles.contains(s_fieldnames_vector[i]);
          if(is_title) {
            fout << '{';
          }
          fout << value.fields.at(s_fieldnames_vector[i]) << '}';
          if(is_title) {
            fout << '}';
          }
        }
      }
      fout << "\n}\n";
    }
  }
  fout.close();

  return 0;
}
