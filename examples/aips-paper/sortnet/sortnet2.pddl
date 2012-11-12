(define (domain sortnet4)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:objects array - :array[4] :integer[0,1])
  (:action cmpswap
    :parameters ?i ?j - :integer[0,3]
    :precondition (< ?i ?j)
    :effect (:when (< array[?j] array[?i])
                   (:set array[?i] array[?j])
                   (:set array[?j] array[?i]))))

(define (problem p4)
  (:domain sortnet4)
  (:init (:set array[0] :in :integer[0,1])
         (:set array[1] :in :integer[0,1])
         (:set array[2] :in :integer[0,1])
         (:set array[3] :in :integer[0,1]))
  (:goal (:and (<= array[0] array[1])
               (<= array[1] array[2])
               (<= array[2] array[3]))))



(define (domain sortnet5)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:objects array - :array[5] :integer[0,1])
  (:action cmpswap
    :parameters ?i ?j - :integer[0,4]
    :precondition (< ?i ?j)
    :effect (:when (< array[?j] array[?i])
                   (:set array[?i] array[?j])
                   (:set array[?j] array[?i]))))

(define (problem p5)
  (:domain sortnet5)
  (:init (:set array[0] :in :integer[0,1])
         (:set array[1] :in :integer[0,1])
         (:set array[2] :in :integer[0,1])
         (:set array[3] :in :integer[0,1])
         (:set array[4] :in :integer[0,1]))
  (:goal (:and (<= array[0] array[1])
               (<= array[1] array[2])
               (<= array[2] array[3])
               (<= array[3] array[4]))))



(define (domain sortnet6)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:objects array - :array[6] :integer[0,1])
  (:action cmpswap
    :parameters ?i ?j - :integer[0,5]
    :precondition (< ?i ?j)
    :effect (:when (< array[?j] array[?i])
                   (:set array[?i] array[?j])
                   (:set array[?j] array[?i]))))

(define (problem p6)
  (:domain sortnet6)
  (:init (:set array[0] :in :integer[0,1])
         (:set array[1] :in :integer[0,1])
         (:set array[2] :in :integer[0,1])
         (:set array[3] :in :integer[0,1])
         (:set array[4] :in :integer[0,1])
         (:set array[5] :in :integer[0,1]))
  (:goal (:and (<= array[0] array[1])
               (<= array[1] array[2])
               (<= array[2] array[3])
               (<= array[3] array[4])
               (<= array[4] array[5]))))



(define (domain sortnet7)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:objects array - :array[7] :integer[0,1])
  (:action cmpswap
    :parameters ?i ?j - :integer[0,6]
    :precondition (< ?i ?j)
    :effect (:when (< array[?j] array[?i])
                   (:set array[?i] array[?j])
                   (:set array[?j] array[?i]))))

(define (problem p7)
  (:domain sortnet7)
  (:init (:set array[0] :in :integer[0,1])
         (:set array[1] :in :integer[0,1])
         (:set array[2] :in :integer[0,1])
         (:set array[3] :in :integer[0,1])
         (:set array[4] :in :integer[0,1])
         (:set array[5] :in :integer[0,1])
         (:set array[6] :in :integer[0,1]))
  (:goal (:and (<= array[0] array[1])
               (<= array[1] array[2])
               (<= array[2] array[3])
               (<= array[3] array[4])
               (<= array[4] array[5])
               (<= array[5] array[6]))))


