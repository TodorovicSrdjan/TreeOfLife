# TreeOfLife

TreeOfLife is a project made for a UpWork client with specific implementation requirements (like use of n-ary trees with connected sibling nodes, predefined formats for IO files, etc.).  
This is a program for answering questions about relationships between some categories on a given set of data. The set contains information about subcategory for each category present in the set (every subcategory for each category, every subcategory for each subcategory and so on).

Supported questions are:
* `DirectSupercategory category`: What is the direct supercategory for *category*?
* `AllSupercategories category`: What are all supercategories for *category*?
* `NumberOfAllSupercategories category`: How many supercategories does *category* have?
* `IsSupercategory category1 category2`: Is *category1* supercategory of *category2*?
* `ClosestCommonSupercategory category1 category2`: Which category is the closest common supercategory for *category1* and *category2*?
* `DirectSubcategories category1`: What are the direct subcategories for *category1*?
* `AllSubcategories category1`: What are all subcategories for *category*?
* `NumberOfAllSubcategories category`: How many subcategories does *category* have?
* `IsSubcategory category1 category2`: Is *category1* subcategory of *category2*?
