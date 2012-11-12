;-------------------------------------------------------------------------------------------;
;
; Small Rural network:
;
;                                                                              l5
;   cb5 X---------------------------------------------------------------------------
;                                                                                  |
;                                                                                  |
;                                      sd6                                         |
;              l15                  ----O----------------X cb6                     O sd4
;               |                   |          l7                                  |
;          sd26 X                   |                                              |
;               |      sd1          |          sd2                sd3              |
;   cb1 0---------------X-----------------------X------------------X----------------
;              l1                  l2                   l3                       l4
;
;-------------------------------------------------------------------------------------------;

(define (problem rsm1)
  (:domain std_worst)
  (:objects cb1 cb5 cb6 sd1 sd2 sd3 sd4 sd6 sd26 - SWITCH
            l1 l2 l3 l4 l5 l7 l15 - LINE)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)
         (:set done false)

         (:set (breaker cb1) true)
         (:set (breaker cb5) true)
         (:set (breaker cb6) true)

         (:set (ext l1 cb1 side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l1 sd26 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd6 side1) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 cb5 side2) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 cb6 side2) true)
         (:set (ext l15 sd26 side2) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:set (closed cb1) false)
         (:set (closed sd4) false)
         (:set (closed sd6) false)

         (:set (faulty l15) :in {true false})
  )
  (:goal (= done true))
)

(define (problem rsm2)
  (:domain std_worst)
  (:objects cb1 cb5 cb6 sd1 sd2 sd3 sd4 sd6 sd26 - SWITCH
            l1 l2 l3 l4 l5 l7 l15 - LINE)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)
         (:set done false)

         (:set (breaker cb1) true)
         (:set (breaker cb5) true)
         (:set (breaker cb6) true)

         (:set (ext l1 cb1 side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l1 sd26 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd6 side1) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 cb5 side2) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 cb6 side2) true)
         (:set (ext l15 sd26 side2) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:set (closed cb1) false)
         (:set (closed sd4) false)
         (:set (closed sd6) false)

         (:set (faulty l3) :in {true false})
         (:set (faulty l15) :in {true false})
  )
  (:goal (= done true))
)

(define (problem rsm3)
  (:domain std_worst)
  (:objects cb1 cb5 cb6 sd1 sd2 sd3 sd4 sd6 sd26 - SWITCH
            l1 l2 l3 l4 l5 l7 l15 - LINE)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)
         (:set done false)

         (:set (breaker cb1) true)
         (:set (breaker cb5) true)
         (:set (breaker cb6) true)

         (:set (ext l1 cb1 side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l1 sd26 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd6 side1) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 cb5 side2) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 cb6 side2) true)
         (:set (ext l15 sd26 side2) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:set (closed cb1) false)
         (:set (closed sd4) false)
         (:set (closed sd6) false)

         (:set (pd_ok sd26) :in {true false})
         (:set (faulty l3) :in {true false})
         (:set (faulty l15) :in {true false})
  )
  (:goal (= done true))
)

(define (problem rsm4)
  (:domain std_worst)
  (:objects cb1 cb5 cb6 sd1 sd2 sd3 sd4 sd6 sd26 - SWITCH
            l1 l2 l3 l4 l5 l7 l15 - LINE)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)
         (:set done false)

         (:set (breaker cb1) true)
         (:set (breaker cb5) true)
         (:set (breaker cb6) true)

         (:set (ext l1 cb1 side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l1 sd26 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd6 side1) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 cb5 side2) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 cb6 side2) true)
         (:set (ext l15 sd26 side2) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:set (closed cb1) false)
         (:set (closed sd4) false)
         (:set (closed sd6) false)

         (:set (ac_mode sd26) :in {ok out liar})
         (:set (pd_ok sd26) :in {true false})
         (:set (faulty l3) :in {true false})
         (:set (faulty l15) :in {true false})
  )
  (:goal (= done true))
)

(define (problem rsm5)
  (:domain std_worst)
  (:objects cb1 cb5 cb6 sd1 sd2 sd3 sd4 sd6 sd26 - SWITCH
            l1 l2 l3 l4 l5 l7 l15 - LINE)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)
         (:set done false)

         (:set (breaker cb1) true)
         (:set (breaker cb5) true)
         (:set (breaker cb6) true)

         (:set (ext l1 cb1 side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l1 sd26 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd6 side1) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 cb5 side2) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 cb6 side2) true)
         (:set (ext l15 sd26 side2) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:set (closed cb1) false)
         (:set (closed sd4) false)
         (:set (closed sd6) false)

         (:set (fd_mode sd26) :in {ok out liar})
         (:set (ac_mode sd26) :in {ok out liar})
         (:set (pd_ok sd26) :in {true false})
         (:set (faulty l3) :in {true false})
         (:set (faulty l15) :in {true false})
  )
  (:goal (= done true))
)

(define (problem rsm6)
  (:domain std_worst)
  (:objects cb1 cb5 cb6 sd1 sd2 sd3 sd4 sd6 sd26 - SWITCH
            l1 l2 l3 l4 l5 l7 l15 - LINE)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)
         (:set done false)

         (:set (breaker cb1) true)
         (:set (breaker cb5) true)
         (:set (breaker cb6) true)

         (:set (ext l1 cb1 side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l1 sd26 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd6 side1) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 cb5 side2) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 cb6 side2) true)
         (:set (ext l15 sd26 side2) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:set (closed cb1) false)
         (:set (closed sd4) false)
         (:set (closed sd6) false)

         (:set (fd_mode sd3)  :in {ok out liar})
         (:set (fd_mode sd26) :in {ok out liar})
         (:set (ac_mode sd26) :in {ok out liar})
         (:set (pd_ok sd26) :in {true false})
         (:set (faulty l3) :in {true false})
         (:set (faulty l15) :in {true false})
  )
  (:goal (= done true))
)

(define (problem rsm7)
  (:domain std_worst)
  (:objects cb1 cb5 cb6 sd1 sd2 sd3 sd4 sd6 sd26 - SWITCH
            l1 l2 l3 l4 l5 l7 l15 - LINE)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)
         (:set done false)

         (:set (breaker cb1) true)
         (:set (breaker cb5) true)
         (:set (breaker cb6) true)

         (:set (ext l1 cb1 side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l1 sd26 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd6 side1) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 cb5 side2) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 cb6 side2) true)
         (:set (ext l15 sd26 side2) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:set (closed cb1) false)
         (:set (closed sd4) false)
         (:set (closed sd6) false)

         (:set (fd_mode sd2)  :in {ok out liar})
         (:set (fd_mode sd3)  :in {ok out liar})
         (:set (fd_mode sd26) :in {ok out liar})
         (:set (ac_mode sd26) :in {ok out liar})
         (:set (pd_ok sd26) :in {true false})
         (:set (faulty l3) :in {true false})
         (:set (faulty l15) :in {true false})
  )
  (:goal (= done true))
)

(define (problem rsm8)
  (:domain std_worst)
  (:objects cb1 cb5 cb6 sd1 sd2 sd3 sd4 sd6 sd26 - SWITCH
            l1 l2 l3 l4 l5 l7 l15 - LINE)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)
         (:set done false)

         (:set (breaker cb1) true)
         (:set (breaker cb5) true)
         (:set (breaker cb6) true)

         (:set (ext l1 cb1 side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l1 sd26 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd6 side1) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 cb5 side2) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 cb6 side2) true)
         (:set (ext l15 sd26 side2) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:set (closed cb1) false)
         (:set (closed sd4) false)
         (:set (closed sd6) false)

         (:set (fd_mode sd1)  :in {ok out liar})
         (:set (fd_mode sd2)  :in {ok out liar})
         (:set (fd_mode sd3)  :in {ok out liar})
         (:set (fd_mode sd26) :in {ok out liar})
         (:set (ac_mode sd26) :in {ok out liar})
         (:set (pd_ok sd26) :in {true false})
         (:set (faulty l3) :in {true false})
         (:set (faulty l15) :in {true false})
  )
  (:goal (= done true))
)
