## RazorLeaf
A static program slicer based on LLVM IR  
<br> <br> <br>
<small>
Ye Jiabin  
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
### ctrl. dep. (cont.)
Using LLVM built-in iterator to get control flow graph
information.
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

