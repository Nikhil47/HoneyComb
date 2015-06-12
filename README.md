# HoneyComb
Word Search in a HoneyComb

HoneyComb is a data structure such that each node is a hexagon, therefore, has at the most six neighbors. Nodes belong to layers,
where each layer has a number of nodes perfectly divisible by 6. Except for the first layer which contains just one node. Each 
node just contains a letter.

The task is to efficiently find a given string in the given HoneyComb. The string comes from a dictionary, which may contain 
multiple strings. Each node, once selected in a search can not be reused again. 

Run: ./a.out honeycomb.txt dictionary.txt

The approach is:
 - First, load dictionary into the main memory. For this purpose, a bucket list is used where each string is sieved according to 
 its starting letter. The bucket list is actually a group of 26 linked lists.
 - Second, load the honeycomb in a similar data structure as above so that whole of the comb is not parsed in every search.
 - Third, using a recursive back-track approach search for strings.
 - Fourth, delete all the entries in the linked list which were not found in the search.
 - Fifth, using merge sort, start sorting the linked list. However, by the end of first iteration, all the entries for that bucket 
 are loaded into an array. And then the sort proceeds on the array.
 - Sixth, print the results and free the memory.
 
Pros:
 - First, the strings created when the dictionary had been loaded in the memory are never copied again. Same strings are used.
 - Second, the graph is not pre-computed. Nodes required for search are created on the fly.

Cons:
 - First, all of the dictionary is loaded into the memory. This can be improved. If the words in the dictionary are longer then
 this approach will become memory intensive.
 - Second, the recursive back-track provides an elegant solution however, if words in the dictionary are longer then the stack,
 might overflow.

Solution:
 - First, files might be used to store the results and strings can be read one by one. 'Yield-enumerate' approach from python.
 - Second, a breadth first search with queue needs to be implemented here.
