(define (domain bmtc_hi)
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

;; medium uncertainty problems, 2 toilets

(define (problem p2_2)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p3_2)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p4_2)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p5_2)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p6_2)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p7_2)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p8_2)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p9_2)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p10_2)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - PACKAGE
            t1 t2 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false }))
  (:goal (:not (armed b))))


;; medium uncertainty problems, 4 toilets

(define (problem p2_4)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p3_4)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p4_4)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p5_4)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p6_4)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p7_4)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p8_4)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p9_4)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p10_4)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - PACKAGE
            t1 t2 t3 t4 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false }))
  (:goal (:not (armed b))))


;; medium uncertainty problems, 6 toilets

(define (problem p2_6)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false })
         (:set (clogged t5) :in { true false })
         (:set (clogged t6) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p3_6)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false })
         (:set (clogged t5) :in { true false })
         (:set (clogged t6) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p4_6)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false })
         (:set (clogged t5) :in { true false })
         (:set (clogged t6) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p5_6)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false })
         (:set (clogged t5) :in { true false })
         (:set (clogged t6) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p6_6)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false })
         (:set (clogged t5) :in { true false })
         (:set (clogged t6) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p7_6)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false })
         (:set (clogged t5) :in { true false })
         (:set (clogged t6) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p8_6)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false })
         (:set (clogged t5) :in { true false })
         (:set (clogged t6) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p9_6)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false })
         (:set (clogged t5) :in { true false })
         (:set (clogged t6) :in { true false }))
  (:goal (:not (armed b))))

(define (problem p10_6)
  (:domain bmtc_hi)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - PACKAGE
            t1 t2 t3 t4 t5 t6 - TOILET)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t1) :in { true false })
         (:set (clogged t2) :in { true false })
         (:set (clogged t3) :in { true false })
         (:set (clogged t4) :in { true false })
         (:set (clogged t5) :in { true false })
         (:set (clogged t6) :in { true false }))
  (:goal (:not (armed b))))
