# Clean References

## Purpose
This is a tool to clean a bibtex file. It i.a....
- adds a header with a reference count and a sorted list of the entry names
- preserves `%` comments above references
- begins comments with `%` followed by white space
- lists references alpabetically
- capitalizes the first letter of entry names
- spells publication types (e.g. `article`) and field keys (e.g. `author`) in lower case characters
- indents field names with two spaces
- uses white space around the `=`
- vertically aligns field keys, values, and `=`
- orders supplied fields according to `author`, `title`, `editor`, `booktitle`, `journal`, `volume`, `number`, `year`, `type`, `pages`, `publisher`, `institution`, and `note`
- drops other fields
- removes white space from `volume`, `number`, `year`, and `pages` values
- separates pages in the `pages` value with a single hyphen
- case-insensitively removes initial `the` from journal names
- wraps `title` and `booktitle` values in doube braces, others in single braces
- removes double white space
- trims white space surrounding field values
- prints messages on incorrectly specified entries to standard output (the console)
- does probably not work with all encodings
- uses C++20

To clarify what publication type etc. refer to, this is the structure of a bibtex entry:
```
@publication_type{entry_name,
  field_key = field_value
}
```

## Use
First, download and compile `clean_references.cpp`, e.g. using GCC with
```bash
g++ -o clean_references -std=c++20 -Wall -Werror -Wpedantic -O3 -march=native clean_references.cpp
```
On Windows, GCC is available via, for instance, MinGW-w64.

Run the resulting binary file with the syntax
```bash
./clean_references <input_file> <output_file>
```
where `<intput_file>` and `<output_file>` are the names of the input and output files respectively. If the two file names are identical, the tool overwrites the input file.

You are free to modify the C++ code and adjust it to your bibtex file layout preferences.

## Example
bibtex_input.bib
```
%economic geography
@article{dueben2023,
    author= {Christian Düben and Melanie Krause},
    title= {{The Emperor's Geography - City Locations, Nature and Institutional Optimisation}},
    abstract= "{The emergence of cities in specific locations depends on both geographical features (such as elevation and proximity to rivers) and institutional factors (such as centrality within an administrative region). In this paper, we analyse the importance of these factors at different levels of the urban hierarchy. To do so, we exploit a unique data set on the locations of cities of different status in imperial China from 221 BCE to 1911 CE, a geographically diverse empire with a long history of centralised rule. Developing a stylised theoretical model, we combine econometrics with machine learning techniques. Our results suggest that the higher a city is in the urban hierarchy, the less important are local geographical features compared to institutional factors. At the lower end of the scale, market towns without government responsibilities are most strongly shaped by geographical characteristics. We also find evidence that many cities of political importance in imperial times still enjoy a special status nowadays, underlining the modern relevance of these historical factors.}",
    journal= {The Economic Journal},
    year= {2023},
    volume= {133},
    number= {651},
    pages= {1067--1105}
}

@book{Wilkinson2013,
Author      = {Endymion Wilkinson},
Title       = {{Chinese History: A New Manual}},
Publisher   = {Cambridge: Harvard University Asia Center for the Harvard-Yenching Institute},
ISBN        = {978-0-674-06715-8},
Year        = {2013},
}
% Modern research  at the intersection of remote sensing and economics
% mention in introduction
@techreport{Ackermann2020,
      author={Klaus Ackermann and Alexey Chernikov and Nandini Anantharama and Miethy Zaman and Paul A Raschky},
      title={{Object Recognition for Economic Development from Daytime Satellite Imagery}},
      year={2020},
      institution={ArXiv},
      type={Working Paper},
      number={2009.05455}
}
```
```bash
./clean_references bibtex_input.bib bibtex_output.bib
```
bibtex_output.bib
```
% 3 references
% Entry Names: Ackermann2020, Dueben2023, Wilkinson2013

% Modern research at the intersection of remote sensing and economics
% mention in introduction
@techreport{Ackermann2020,
  author      = {Klaus Ackermann and Alexey Chernikov and Nandini Anantharama and Miethy Zaman and Paul A Raschky},
  title       = {{Object Recognition for Economic Development from Daytime Satellite Imagery}},
  number      = {2009.05455},
  year        = {2020},
  type        = {Working Paper},
  institution = {ArXiv}
}
% economic geography
@article{Dueben2023,
  author      = {Christian Düben and Melanie Krause},
  title       = {{The Emperor's Geography - City Locations, Nature and Institutional Optimisation}},
  volume      = {133},
  number      = {651},
  year        = {2023},
  pages       = {1067-1105}
}
@book{Wilkinson2013,
  author      = {Endymion Wilkinson},
  title       = {{Chinese History: A New Manual}},
  year        = {2013},
  publisher   = {Cambridge: Harvard University Asia Center for the Harvard-Yenching Institute}
}
```

## Contributions
Further contributions from my side to this tool are unlikely. However, you are welcome to incorporate updates using pull requests.
