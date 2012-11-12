(define (domain bmtc_lo)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:types PACKAGE BOMB TOILET)
  (:functions (bomb PACKAGE BOMB)
              (in BOMB PACKAGE))
  (:predicates (armed BOMB)
               (clogged TOILET))
  (:objects nil - BOMB)

  (:ramification ram1
    :parameters ?p - PACKAGE ?b - BOMB
    :effect (:when (= (in ?b) ?p) (:set (bomb ?p) ?b))
            (:when (:and (:not (= ?b nil)) (:not (= (in ?b) ?p))) (:set (bomb ?p) nil))
  )

  (:action dunk
    :parameters ?p - PACKAGE ?t - TOILET
    :precondition (:not (clogged ?t))
    :effect (:set (armed (bomb ?p)) false)
            (:set (clogged ?t) true)
  )

  (:action flush
    :parameters ?t - TOILET
    :effect (:set (clogged ?t) false)
  )
)

;; low uncertainty problems, 2 toilets

(define (problem p2_2)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false))
  (:goal (:not (armed b))))

(define (problem p3_2)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false))
  (:goal (:not (armed b))))

(define (problem p4_2)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false))
  (:goal (:not (armed b))))

(define (problem p5_2)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false))
  (:goal (:not (armed b))))

(define (problem p6_2)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false))
  (:goal (:not (armed b))))

(define (problem p7_2)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false))
  (:goal (:not (armed b))))

(define (problem p8_2)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false))
  (:goal (:not (armed b))))

(define (problem p9_2)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false))
  (:goal (:not (armed b))))

(define (problem p10_2)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false))
  (:goal (:not (armed b))))


;; limiting


(define (problem p14_2)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false))
  (:goal (:not (armed b))))


(define (problem p15_2)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 p15 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 p15 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false))
  (:goal (:not (armed b))))


;; low uncertainty problems, 4 toilets

(define (problem p2_4)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false))
  (:goal (:not (armed b))))

(define (problem p3_4)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false))
  (:goal (:not (armed b))))

(define (problem p4_4)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false))
  (:goal (:not (armed b))))

(define (problem p5_4)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false))
  (:goal (:not (armed b))))

(define (problem p6_4)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false))
  (:goal (:not (armed b))))

(define (problem p7_4)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false))
  (:goal (:not (armed b))))

(define (problem p8_4)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false))
  (:goal (:not (armed b))))

(define (problem p9_4)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false))
  (:goal (:not (armed b))))

(define (problem p10_4)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false))
  (:goal (:not (armed b))))

;; limiting

(define (problem p12_4)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false))
  (:goal (:not (armed b))))




;; low uncertainty problems, 6 toilets

(define (problem p2_6)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false)
         (:set (clogged t5) false)
         (:set (clogged t6) false))
  (:goal (:not (armed b))))

(define (problem p3_6)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false)
         (:set (clogged t5) false)
         (:set (clogged t6) false))
  (:goal (:not (armed b))))

(define (problem p4_6)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false)
         (:set (clogged t5) false)
         (:set (clogged t6) false))
  (:goal (:not (armed b))))

(define (problem p5_6)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false)
         (:set (clogged t5) false)
         (:set (clogged t6) false))
  (:goal (:not (armed b))))

(define (problem p6_6)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false)
         (:set (clogged t5) false)
         (:set (clogged t6) false))
  (:goal (:not (armed b))))

(define (problem p7_6)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false)
         (:set (clogged t5) false)
         (:set (clogged t6) false))
  (:goal (:not (armed b))))

(define (problem p8_6)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false)
         (:set (clogged t5) false)
         (:set (clogged t6) false))
  (:goal (:not (armed b))))

(define (problem p9_6)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false)
         (:set (clogged t5) false)
         (:set (clogged t6) false))
  (:goal (:not (armed b))))

(define (problem p10_6)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false)
         (:set (clogged t5) false)
         (:set (clogged t6) false))
  (:goal (:not (armed b))))


;; limiting


(define (problem p11_6)
  (:domain bmtc_lo)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) false)
         (:set (clogged t2) false)
         (:set (clogged t3) false)
         (:set (clogged t4) false)
         (:set (clogged t5) false)
         (:set (clogged t6) false))
  (:goal (:not (armed b))))
