(define (domain bt)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:types PACKAGE BOMB)
  (:functions (bomb PACKAGE BOMB)
              (in BOMB PACKAGE))
  (:predicates (armed BOMB))
  (:objects nil - BOMB)

  (:ramification ram1
    :parameters ?p - PACKAGE 
                ?b - BOMB
    :effect (:when (= (in ?b) ?p) (:set (bomb ?p) ?b))
            (:when (:and (:not (= ?b nil)) (:not (= (in ?b) ?p))) (:set (bomb ?p) nil))
  )

  (:action dunk
    :parameters ?pkg - PACKAGE
    :effect (:set (armed (bomb ?pkg)) false)
  )
)

(define (problem p2)
  (:domain bt)
  (:objects b - BOMB
            p1 p2 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 })
         (:set (armed b) true)
         (:set (armed nil) false))
  (:goal (:not (armed b))))

(define (problem p3)
  (:domain bt)
  (:objects b - BOMB
            p1 p2 p3 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 })
         (:set (armed b) true)
         (:set (armed nil) false))
  (:goal (:not (armed b))))

(define (problem p4)
  (:domain bt)
  (:objects b - BOMB
            p1 p2 p3 p4 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 })
         (:set (armed b) true)
         (:set (armed nil) false))
  (:goal (:not (armed b))))

(define (problem p5)
  (:domain bt)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 })
         (:set (armed b) true)
         (:set (armed nil) false))
  (:goal (:not (armed b))))

(define (problem p6)
  (:domain bt)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 })
         (:set (armed b) true)
         (:set (armed nil) false))
  (:goal (:not (armed b))))

(define (problem p7)
  (:domain bt)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 })
         (:set (armed b) true)
         (:set (armed nil) false))
  (:goal (:not (armed b))))

(define (problem p8)
  (:domain bt)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 })
         (:set (armed b) true)
         (:set (armed nil) false))
  (:goal (:not (armed b))))

(define (problem p9)
  (:domain bt)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 })
         (:set (armed b) true)
         (:set (armed nil) false))
  (:goal (:not (armed b))))

(define (problem p10)
  (:domain bt)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 })
         (:set (armed b) true)
         (:set (armed nil) false))
  (:goal (:not (armed b))))




;; limits

(define (problem p17)
  (:domain bt)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 p15 p16 p17 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 p15
                            p16 p17 })
         (:set (armed b) true)
         (:set (armed nil) false))
  (:goal (:not (armed b))))

(define (problem p18)
  (:domain bt)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 p15 p16 p17 p18 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 p15
                            p16 p17 p18 })
         (:set (armed b) true)
         (:set (armed nil) false))
  (:goal (:not (armed b))))

(define (problem p30)
  (:domain bt)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10
            p11 p12 p13 p14 p15 p16 p17 p18 p19 p20
            p21 p22 p23 p24 p25 p26 p27 p28 p29 p30 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10
                            p11 p12 p13 p14 p15 p16 p17 p18 p19 p20
                            p21 p22 p23 p24 p25 p26 p27 p28 p29 p30 })
         (:set (armed b) true)
         (:set (armed nil) false))
  (:goal (:not (armed b))))
