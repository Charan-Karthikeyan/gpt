(define (domain medical)
  (:model (:dynamics :non-deterministic) (:feedback :partial))
  (:objects ill - :integer[0,5] 
            stain_result - :integer[0,3]
            high_cell_count - :boolean
            dead - :boolean)

  (:action stain
    :precondition (= dead false)
     :effect (:when (:or (= ill 3) (= ill 4)) (:set stain_result 1))
             (:when (:or (= ill 1) (= ill 2)) (:set stain_result 2))
             (:when (= ill 5) (:set stain_result 3)))

  (:action count_white_cells
    :precondition (= dead false)
    :effect (:when (:or (= ill 1) (= ill 3) (= ill 5))
              (:set high_cell_count true)))

  (:action inspect
    :precondition (= dead false)
    :observation stain_result)

  (:action analyze_blood
    :precondition (= dead false)
    :observation high_cell_count)

  (:action medicate
    :parameters ?i - :integer[0,5]
    :precondition (:and (= dead false) (:not (= ?i 0)))
    :effect (:when (= ill ?i) (:set ill 0))
            (:when (:not (= ill ?i)) (:set dead true)))
)

(define (problem p1)
  (:domain medical)
  (:init (:set stain_result 0)
         (:set high_cell_count false)
         (:set ill 1)
         (:set dead false))
  (:goal (:and (= ill 0) (:not (= dead true)))))

(define (problem p2)
  (:domain medical)
  (:init (:set stain_result 0)
         (:set high_cell_count false)
         (:set ill :in { 1 2 })
         (:set dead false))
  (:goal (:and (= ill 0) (:not (= dead true)))))

(define (problem p3)
  (:domain medical)
  (:init (:set stain_result 0)
         (:set high_cell_count false)
         (:set ill :in { 1 2 3 })
         (:set dead false))
  (:goal (:and (= ill 0) (:not (= dead true)))))

(define (problem p4)
  (:domain medical)
  (:init (:set stain_result 0)
         (:set high_cell_count false)
         (:set ill :in { 1 2 3 4 })
         (:set dead false))
  (:goal (:and (= ill 0) (:not (= dead true)))))

(define (problem p5)
  (:domain medical)
  (:init (:set stain_result 0)
         (:set high_cell_count false)
         (:set ill :in { 1 2 3 4 5 })
         (:set dead false))
  (:goal (:and (= ill 0) (:not (= dead true)))))
