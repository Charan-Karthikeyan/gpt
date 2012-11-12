;-------------------------------------------------------------------------;
;
; Linear topology:
;
;          l1       l2   
;   cb X--------X--------X--- ... ---X
;              sd1      sd2         sdn
;
; Problems:
;
;   ln_em : problem with n and exactly m faulty lines
;   ln_mm : problem with n and at most m faulty lines
;
;-------------------------------------------------------------------------;

;
; ---- 2 lines ----
;

(define (problem l2_e0)
  (:domain mbp_worst)
  (:objects l1 l2 - LINE
            cb sd1 sd2 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 0)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l2_e1)
  (:domain mbp_worst)
  (:objects l1 l2 - LINE
            cb sd1 sd2 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 1)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l2_e2)
  (:domain mbp_worst)
  (:objects l1 l2 - LINE
            cb sd1 sd2 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 2)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

;
; ---- 4 lines ----
;

(define (problem l4_e0)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 - LINE
            cb sd1 sd2 sd3 sd4 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 0)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l4_e2)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 - LINE
            cb sd1 sd2 sd3 sd4 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 2)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l4_e4)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 - LINE
            cb sd1 sd2 sd3 sd4 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 4)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

;
; ---- 6 lines ----
;

(define (problem l6_e0)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 0)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l6_e2)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 2)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l6_e4)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 4)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l6_e6)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 6)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

;
; ---- 8 lines ----
;

(define (problem l8_e0)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 sd7 side1) true)
         (:set (ext l8 sd7 side2) true)
         (:set (ext l8 sd8 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 0)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l8_e2)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 sd7 side1) true)
         (:set (ext l8 sd7 side2) true)
         (:set (ext l8 sd8 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 2)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l8_e4)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 sd7 side1) true)
         (:set (ext l8 sd7 side2) true)
         (:set (ext l8 sd8 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 4)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l8_e6)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 sd7 side1) true)
         (:set (ext l8 sd7 side2) true)
         (:set (ext l8 sd8 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 6)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l8_e8)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 sd7 side1) true)
         (:set (ext l8 sd7 side2) true)
         (:set (ext l8 sd8 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 8)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

;
; ---- 10 lines ----
;

(define (problem l10_e0)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 l9 l10 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 sd9 sd10 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 sd7 side1) true)
         (:set (ext l8 sd7 side2) true)
         (:set (ext l8 sd8 side1) true)
         (:set (ext l9 sd8 side2) true)
         (:set (ext l9 sd9 side1) true)
         (:set (ext l10 sd9 side2) true)
         (:set (ext l10 sd10 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 0)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l10_e2)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 l9 l10 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 sd9 sd10 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 sd7 side1) true)
         (:set (ext l8 sd7 side2) true)
         (:set (ext l8 sd8 side1) true)
         (:set (ext l9 sd8 side2) true)
         (:set (ext l9 sd9 side1) true)
         (:set (ext l10 sd9 side2) true)
         (:set (ext l10 sd10 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 2)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l10_e4)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 l9 l10 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 sd9 sd10 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 sd7 side1) true)
         (:set (ext l8 sd7 side2) true)
         (:set (ext l8 sd8 side1) true)
         (:set (ext l9 sd8 side2) true)
         (:set (ext l9 sd9 side1) true)
         (:set (ext l10 sd9 side2) true)
         (:set (ext l10 sd10 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 4)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l10_e6)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 l9 l10 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 sd9 sd10 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 sd7 side1) true)
         (:set (ext l8 sd7 side2) true)
         (:set (ext l8 sd8 side1) true)
         (:set (ext l9 sd8 side2) true)
         (:set (ext l9 sd9 side1) true)
         (:set (ext l10 sd9 side2) true)
         (:set (ext l10 sd10 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 6)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l10_e8)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 l9 l10 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 sd9 sd10 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 sd7 side1) true)
         (:set (ext l8 sd7 side2) true)
         (:set (ext l8 sd8 side1) true)
         (:set (ext l9 sd8 side2) true)
         (:set (ext l9 sd9 side1) true)
         (:set (ext l10 sd9 side2) true)
         (:set (ext l10 sd10 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 8)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

(define (problem l10_e10)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 l9 l10 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 sd9 sd10 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 sd7 side1) true)
         (:set (ext l8 sd7 side2) true)
         (:set (ext l8 sd8 side1) true)
         (:set (ext l9 sd8 side2) true)
         (:set (ext l9 sd9 side1) true)
         (:set (ext l10 sd9 side2) true)
         (:set (ext l10 sd10 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 10)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

;
; ---- 12 lines ----
;

(define (problem l12_e1)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 l9 l10 l11 l12 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 sd9 sd10 sd11 sd12 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 sd7 side1) true)
         (:set (ext l8 sd7 side2) true)
         (:set (ext l8 sd8 side1) true)
         (:set (ext l9 sd8 side2) true)
         (:set (ext l9 sd9 side1) true)
         (:set (ext l10 sd9 side2) true)
         (:set (ext l10 sd10 side1) true)
         (:set (ext l11 sd10 side2) true)
         (:set (ext l11 sd11 side1) true)
         (:set (ext l12 sd11 side2) true)
         (:set (ext l12 sd12 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 1)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

;
; ---- 14 lines ----
;

(define (problem l14_e1)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 l9 l10 l11 l12 l13 l14 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 sd9 sd10 sd11 sd12
            sd13 sd14 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 sd7 side1) true)
         (:set (ext l8 sd7 side2) true)
         (:set (ext l8 sd8 side1) true)
         (:set (ext l9 sd8 side2) true)
         (:set (ext l9 sd9 side1) true)
         (:set (ext l10 sd9 side2) true)
         (:set (ext l10 sd10 side1) true)
         (:set (ext l11 sd10 side2) true)
         (:set (ext l11 sd11 side1) true)
         (:set (ext l12 sd11 side2) true)
         (:set (ext l12 sd12 side1) true)
         (:set (ext l13 sd12 side2) true)
         (:set (ext l13 sd13 side1) true)
         (:set (ext l14 sd13 side2) true)
         (:set (ext l14 sd14 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 1)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)

;
; ---- 16 lines ----
;

(define (problem l16_e1)
  (:domain mbp_worst)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 l9 l10 l11 l12 l13 l14 l15 l16 - LINE
            cb sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 sd9 sd10 sd11 sd12
            sd13 sd14 sd15 sd16 - SWITCH)
  (:init (:set (opposite side1) side2)
         (:set (opposite side2) side1)
         (:set need_wait true)

         (:set (breaker cb) true)
         (:set (ext l1 cb side2) true)
         (:set (ext l1 sd1 side1) true)
         (:set (ext l2 sd1 side2) true)
         (:set (ext l2 sd2 side1) true)
         (:set (ext l3 sd2 side2) true)
         (:set (ext l3 sd3 side1) true)
         (:set (ext l4 sd3 side2) true)
         (:set (ext l4 sd4 side1) true)
         (:set (ext l5 sd4 side2) true)
         (:set (ext l5 sd5 side1) true)
         (:set (ext l6 sd5 side2) true)
         (:set (ext l6 sd6 side1) true)
         (:set (ext l7 sd6 side2) true)
         (:set (ext l7 sd7 side1) true)
         (:set (ext l8 sd7 side2) true)
         (:set (ext l8 sd8 side1) true)
         (:set (ext l9 sd8 side2) true)
         (:set (ext l9 sd9 side1) true)
         (:set (ext l10 sd9 side2) true)
         (:set (ext l10 sd10 side1) true)
         (:set (ext l11 sd10 side2) true)
         (:set (ext l11 sd11 side1) true)
         (:set (ext l12 sd11 side2) true)
         (:set (ext l12 sd12 side1) true)
         (:set (ext l13 sd12 side2) true)
         (:set (ext l13 sd13 side1) true)
         (:set (ext l14 sd13 side2) true)
         (:set (ext l14 sd14 side1) true)
         (:set (ext l15 sd14 side2) true)
         (:set (ext l15 sd15 side1) true)
         (:set (ext l16 sd15 side2) true)
         (:set (ext l16 sd16 side1) true)

         (:foreach ?x - SWITCH
            (:set (closed ?x) true)
            (:set (fd_mode ?x) ok)
            (:set (pd_ok ?x) true)
            (:set (ac_mode ?x) ok)
         )

         (:foreach ?l - LINE
            (:set (faulty ?l) :in { true false })
         )

         (= (:sum ?l - LINE (:if (faulty ?l) 1 0)) 1)
  )
  (:goal (:and (= need_wait false)
               (:forall ?l - LINE (:or (:not (feedable_line ?l)) (fed_line ?l)))))
)
