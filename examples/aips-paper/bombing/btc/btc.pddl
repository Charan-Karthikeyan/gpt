(define (domain btc)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:types PACKAGE BOMB)
  (:functions (bomb PACKAGE BOMB)
              (in BOMB PACKAGE))
  (:predicates (armed BOMB))
  (:objects nil - BOMB clogged - :boolean)

  (:ramification ram1
    :parameters ?p - PACKAGE ?b - BOMB
    :effect (:when (= (in ?b) ?p) (:set (bomb ?p) ?b))
            (:when (:and (:not (= ?b nil)) (:not (= (in ?b) ?p))) (:set (bomb ?p) nil))
  )

  (:action dunk
    :parameters ?p - PACKAGE
    :precondition (= clogged false)
    :effect (:set (armed (bomb ?p)) false)
            (:set clogged true)
  )

  (:action flush
    :effect (:set clogged false)
  )
)

(define (problem p2)
  (:domain btc)
  (:objects b - BOMB
            p1 p2 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set clogged false))
  (:goal (:not (armed b))))

(define (problem p3)
  (:domain btc)
  (:objects b - BOMB
            p1 p2 p3 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set clogged false))
  (:goal (:not (armed b))))

(define (problem p4)
  (:domain btc)
  (:objects b - BOMB
            p1 p2 p3 p4 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set clogged false))
  (:goal (:not (armed b))))

(define (problem p5)
  (:domain btc)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set clogged false))
  (:goal (:not (armed b))))

(define (problem p6)
  (:domain btc)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6  - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set clogged false))
  (:goal (:not (armed b))))

(define (problem p7)
  (:domain btc)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set clogged false))
  (:goal (:not (armed b))))

(define (problem p8)
  (:domain btc)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set clogged false))
  (:goal (:not (armed b))))

(define (problem p9)
  (:domain btc)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set clogged false))
  (:goal (:not (armed b))))

(define (problem p10)
  (:domain btc)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set clogged false))
  (:goal (:not (armed b))))



;; limiting




(define (problem p16)
  (:domain btc)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 p15 p16 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13
                            p14 p15 p16 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set clogged false))
  (:goal (:not (armed b))))

