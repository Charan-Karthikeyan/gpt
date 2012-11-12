(define (domain btcs)
  (:model (:dynamics :non-deterministic) (:feedback :partial))
  (:types PACKAGE BOMB TOILET)
  (:functions (bomb PACKAGE BOMB)
              (in BOMB PACKAGE)
  )
  (:predicates (armed BOMB)
               (clogged TOILET)
               (explosive_odor PACKAGE)
  )
  (:objects nil - BOMB t - TOILET)

  (:ramification ram1
    :parameters ?p - PACKAGE
                ?b - BOMB
    :effect (:when (= (in ?b) ?p) (:set (bomb ?p) ?b))
            (:when (:and (:not (= ?b nil)) (:not (= (in ?b) ?p))) (:set (bomb ?p) nil))
  )

  (:ramification ram2
     :parameters ?p - PACKAGE
                 ?b - BOMB
     :effect (:when (= (in ?b) ?p) 
                    (:set (explosive_odor ?p) true))
             (:when (:and (:not (= ?b nil)) (:not (= (in ?b) ?p)))
                    (:set (explosive_odor ?p) false))
  )

  (:action dunk
    :parameters ?p - PACKAGE
    :precondition (:not (clogged t))
    :effect (:set (armed (bomb ?p)) false)
            (:set (clogged t) true)
  )

  (:action flush
    :effect (:set (clogged t) false)
  )

  (:action sniff
     :parameters ?p - PACKAGE
     :observation (explosive_odor ?p)
  )
)


(define (problem p2)
  (:domain btcs)
  (:objects b - BOMB
            p1 p2 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))

(define (problem p4)
  (:domain btcs)
  (:objects b - BOMB
            p1 p2 p3 p4 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))

(define (problem p6)
  (:domain btcs)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))

(define (problem p8)
  (:domain btcs)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))

(define (problem p10)
  (:domain btcs)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))


(define (domain btcs2)
  (:model (:dynamics :non-deterministic) (:feedback :partial))
  (:types PACKAGE BOMB TOILET)
  (:functions (bomb PACKAGE BOMB)
              (in BOMB PACKAGE)
  )
  (:predicates (armed BOMB)
               (clogged TOILET)
               (explosive_odor PACKAGE)
               (metallic_content PACKAGE)
  )
  (:objects nil - BOMB t - TOILET)

  (:ramification ram1
    :parameters ?p - PACKAGE
                ?b - BOMB
    :effect (:when (= (in ?b) ?p) (:set (bomb ?p) ?b))
            (:when (:and (:not (= ?b nil)) (:not (= (in ?b) ?p))) (:set (bomb ?p) nil))
  )

  (:ramification ram2
     :parameters ?p - PACKAGE
                 ?b - BOMB
     :effect (:when (= (in ?b) ?p) 
                    (:set (explosive_odor ?p) true)
                    (:set (metallic_content ?p) true))
             (:when (:and (:not (= ?b nil)) (:not (= (in ?b) ?p)))
                    (:set (explosive_odor ?p) false)
                    (:set (metallic_content ?p) false))
  )

  (:action dunk
    :parameters ?p - PACKAGE
    :precondition (:not (clogged t))
    :effect (:set (armed (bomb ?p)) false)
            (:set (clogged t) true)
  )

  (:action flush
    :effect (:set (clogged t) false)
  )

  (:action sniff
     :parameters ?p - PACKAGE
     :observation (explosive_odor ?p)
  )

  (:action detect_metal
     :parameters ?p - PACKAGE
     :observation (metallic_content ?p)
  )
)


(define (problem p2_2)
  (:domain btcs2)
  (:objects b - BOMB
            p1 p2 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))

(define (problem p4_2)
  (:domain btcs2)
  (:objects b - BOMB
            p1 p2 p3 p4 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))

(define (problem p6_2)
  (:domain btcs2)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))

(define (problem p8_2)
  (:domain btcs2)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))

(define (problem p10_2)
  (:domain btcs2)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))




(define (domain btcs4)
  (:model (:dynamics :non-deterministic) (:feedback :partial))
  (:types PACKAGE BOMB TOILET)
  (:functions (bomb PACKAGE BOMB)
              (in BOMB PACKAGE)
  )
  (:predicates (armed BOMB)
               (clogged TOILET)
               (explosive_odor PACKAGE)
               (metallic_content PACKAGE)
               (tic_tac_noise PACKAGE)
               (bomb_inside_label PACKAGE)
  )
  (:objects nil - BOMB t - TOILET)

  (:ramification ram1
    :parameters ?p - PACKAGE
                ?b - BOMB
    :effect (:when (= (in ?b) ?p) (:set (bomb ?p) ?b))
            (:when (:and (:not (= ?b nil)) (:not (= (in ?b) ?p))) (:set (bomb ?p) nil))
  )

  (:ramification ram2
     :parameters ?p - PACKAGE
                 ?b - BOMB
     :effect (:when (= (in ?b) ?p) 
                    (:set (explosive_odor ?p) true)
                    (:set (metallic_content ?p) true)
                    (:set (tic_tac_noise ?p) true)
                    (:set (bomb_inside_label ?p) true))
             (:when (:and (:not (= ?b nil)) (:not (= (in ?b) ?p)))
                    (:set (explosive_odor ?p) false)
                    (:set (metallic_content ?p) false)
                    (:set (tic_tac_noise ?p) false)
                    (:set (bomb_inside_label ?p) false))
  )

  (:action dunk
    :parameters ?p - PACKAGE
    :precondition (:not (clogged t))
    :effect (:set (armed (bomb ?p)) false)
            (:set (clogged t) true)
  )

  (:action flush
    :effect (:set (clogged t) false)
  )

  (:action sniff
     :parameters ?p - PACKAGE
     :observation (explosive_odor ?p)
  )

  (:action detect_metal
     :parameters ?p - PACKAGE
     :observation (metallic_content ?p)
  )

  (:action hear_package
     :parameters ?p - PACKAGE
     :observation (tic_tac_noise ?p)
  )

  (:action read_label
     :parameters ?p - PACKAGE
     :observation (bomb_inside_label ?p)
  )
)


(define (problem p2_4)
  (:domain btcs4)
  (:objects b - BOMB
            p1 p2 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))

(define (problem p4_4)
  (:domain btcs4)
  (:objects b - BOMB
            p1 p2 p3 p4 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))

(define (problem p6_4)
  (:domain btcs4)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))

(define (problem p8_4)
  (:domain btcs4)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))

(define (problem p10_4)
  (:domain btcs4)
  (:objects b - BOMB
            p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - PACKAGE)
  (:init (:set (in b) :in { p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 })
         (:set (armed b) true)
         (:set (armed nil) false)
         (:set (clogged t) false))
  (:goal (:not (armed b))))
