LINKS
=====
- External Memory Three-Sided Range Reporting and Top-k Queries with Sublogarithmic Updates∗
  http://arxiv.org/pdf/1509.08240v1.pdf

BOOKS
=====
- Computational Geometry - Algorithms and Applications Third Edition
  by Mark de Berg et al.

- Introduction to Algorithms by Cormen et al. Pages 484-504 on B-trees

VIDEOS
===== 
- NPTEL lecture on internal orthogonal range searching
  https://www.youtube.com/watch?v=WMKoe_b3IDE
  
- NPTEL lecture on internal PST
  https://www.youtube.com/watch?v=KO5r0BSRmF4

NOTES
=====
- Use tournament tree as pq
- Use O(B^{1+e}) as Cv in Arge
- Ask Kenn & Færgemand about top-k selection in linear time
- Meassure I/O's atomically
- Cache-Grind
- Rolf Fagerberg, External buffered B-tree by master's students
- Use sorted vector as pq
- xfig, ipe, tex from arxiv
- Decorator pattern: Test individual methods

ERRORS IN ARTICLE
=================
- Report
  - (Pv U Iv) \cap Q . (Pv should have been reported in Cv, except for root)
  - (Cv \cap Q) \ Dv (Dv should not contain any relevant points on path)
- Remove all points in Dv with y-value larger than the smallest y-value in X as these cannot cancel any updates further above.
- min_y should be max_y at some places
- Deletions might cause node to remain underflowed after siftup.
- Inserts and deletes should not cancel each other before they hit a point buffer
- Spelling mistake of construction in 5. Global rebuilding
- Weird sentence: To be able to filter out deleted points etc., we maintain the buffer Pv, Iv and Dv in lexicographically sorted. Section 5.
- When does a leaf's point buffer overflow? B or B/2?
- Point buffer of children can go empty during underflowing point buffer handling. Child should be refilled before continuing.