## RazorLeaf
A static program slicer based on LLVM IR  
<br> <br> <br>
<small>
叶家彬  
5090379125
</small>
;;;
### Agenda

- architecture
- program dependence graph
  - control dependence
  - data dependence
- slicing algorithm  

;;;
architecture
------------
;;
### overview of LLVM system
<!-- graph -->
<object type="image/svg+xml" data="img/arch-1.svg">
</object>
;;
### design of RazorLeaf
<object type="image/svg+xml" data="img/arch-2.svg">
</object>
;;;
program dependence graph
------------------------
;;
### control dependence
  
1. Gets the CFG of the program
2. Constructs post dominator tree
3. Calculates control dep. relationship
4. Constructs control dep. graph 

;;
### ctrl. dep. (CFG)
Using LLVM built-in iterator to get   
control flow graph information.
<pre>
<code class="cpp">
    #include "llvm/Support/CFG.h"
    BasicBlock *BB = ...;
    
    for (pred_iterator PI = pred_begin(BB), E = pred_end(BB); 
      PI != E; ++PI) {
      BasicBlock *Pred = *PI;
      // ...
    }

</code></pre>
    
All nodes with two successors have attributes
"T" and "F" associated with outgoing edges.
;;
### ctrl. dep. (CFG in Practice)

    br i1 <cond>, label <iftrue>, label <iffalse>
  
    switch <intty> <value>, label <defaultdest> [ <intty> <val>, label <dest> ... ]


- The basicblock whose terminator inst. is a *switch* 
inst. may have more than two successors.
;;
### ctrl. dep. (PDT)
  
Computing post-dominators in the CFG
is equivalent to computing dominators in
the reverse CFG.  

    $ opt -postdomtree
  
Actually, we use built-in analysis pass   
to get the post dominator tree.

;;
### ctrl. dep. (final stage.)

1. Let set *S* = all edges(A,B) in cfg labeled "T" or "F" such that B is not an ancestor of A in the post-dominsator tree.

2. Let *L* denote the least common ancestor of A and B in the post-dominator tree. By construction, we cannot have *L* equal B.

3. Either *L* is A or *L* is the parent of A in the post-dominator tree.
  * Case *L* = parent of A. All nodes in *L* -> B (!*L*) make control dependent on A.
  * Case *L* = A. All nodes in A -> B make control dependent on A. _(A has loop dependence)_

4. Contructs the control dependence graph.

;;
### data dependence

Flow dependence in LLVM  
  
<pre><code class="avrasm">
    %1 = load i32* %i, align 4 ; Def %1
    %2 = sdiv i32 %1, 2        ; Use %1

</code></pre>  

* loop independent
* loop carried
  - a backedge to the predicate of the Loop
  - both instructions are enclosed in the Loop
;;

### alias in data dependence

* Crucial in memory dependence
* Must/May/No alias response
* Avaliable and may reliable AA pass in LLVM 3.2
  - basic-aa
  - globalsmodref-aa
  - scev-aa

;;;

### slicing algorithm 

* Reduces to a graph reachable problem

A worklist algorithm :  
  
<pre><code class="ruby">
    1. G: PDG; S: Slicing Criterion; WorkList; v, w : vertices in G
    2. WorkList := S
    3. while WorkList is not empty
    4.   Select and remove vertex v from WorkList
    5.   Mark v
    6.   for each unmarked w such that w -> v (c,f) in E(G)
    7.     Insert w Into WorkList
    8.   end for
    9. end while

</code></pre>  

;;;

THANKS
======
