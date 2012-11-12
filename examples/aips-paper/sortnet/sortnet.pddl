(define (domain sortnet2)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:objects array - :array[2] :integer[1,2])

  (:action cmpswap
    :parameters ?i ?j - :integer[0,1]
    :precondition (< ?i ?j)
    :effect (:when (< array[?j] array[?i])
                   (:set array[?i] array[?j])
                   (:set array[?j] array[?i]))))


(define (problem p2)
  (:domain sortnet2)
  (:init (:set array[0] :in :integer[1,2])
         (:set array[1] :in :integer[1,2]
                            :assert (:not (= array[0] array[1]))))
  (:goal (< array[0] array[1])))



(define (domain sortnet3)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:objects array - :array[3] :integer[1,3])

  (:action cmpswap
    :parameters ?i ?j - :integer[0,2]
    :precondition (< ?i ?j)
    :effect (:when (< array[?j] array[?i])
                   (:set array[?i] array[?j])
                   (:set array[?j] array[?i]))))


(define (problem p3)
  (:domain sortnet3)
  (:init (:set array[0] :in :integer[1,3])
         (:set array[1] :in :integer[1,3]
                            :assert (:not (= array[0] array[1])))
         (:set array[2] :in :integer[1,3]
                            :assert (:and (:not (= array[0] array[2]))
                                          (:not (= array[1] array[2])))))
  (:goal (:and (< array[0] array[1])
               (< array[1] array[2]))))





(define (domain sortnet4)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:objects array - :array[4] :integer[1,4])

  (:ramification ram1
    :formula (:and (:not (= array[0] array[1]))
                   (:not (= array[0] array[2]))
                   (:not (= array[0] array[3]))
                   (:not (= array[1] array[2]))
                   (:not (= array[1] array[3]))
                   (:not (= array[2] array[3]))))

  (:action cmpswap
    :parameters ?i ?j - :integer[0,3]
    :precondition (< ?i ?j)
    :effect (:when (< array[?j] array[?i])
                   (:set array[?i] array[?j])
                   (:set array[?j] array[?i]))))


(define (problem p4)
  (:domain sortnet4)
  (:init (:set array[0] :in :integer[1,4])
         (:set array[1] :in :integer[1,4])
         (:set array[2] :in :integer[1,4])
         (:set array[3] :in :integer[1,4]))
  (:goal (:and (< array[0] array[1])
               (< array[1] array[2])
               (< array[2] array[3]))))




(define (domain sortnet5)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:objects array - :array[5] :integer[1,5])

  (:action cmpswap
    :parameters ?i ?j - :integer[0,4]
    :precondition (< ?i ?j)
    :effect (:when (< array[?j] array[?i])
                   (:set array[?i] array[?j])
                   (:set array[?j] array[?i]))))


(define (problem p5)
  (:domain sortnet5)
  (:init (:set array[0] :in :integer[1,5])
         (:set array[1] :in :integer[1,5]
                            :assert (:not (= array[0] array[1])))
         (:set array[2] :in :integer[1,5]
                            :assert (:and (:not (= array[0] array[2]))
                                          (:not (= array[1] array[2]))))
         (:set array[3] :in :integer[1,5]
                            :assert (:and (:not (= array[0] array[3]))
                                          (:not (= array[1] array[3]))
                                          (:not (= array[2] array[3]))))
         (:set array[4] :in :integer[1,5]
                            :assert (:and (:not (= array[0] array[4]))
                                          (:not (= array[1] array[4]))
                                          (:not (= array[2] array[4]))
                                          (:not (= array[3] array[4])))))
  (:goal (:and (< array[0] array[1])
               (< array[1] array[2])
               (< array[2] array[3])
               (< array[3] array[4]))))




(define (domain sortnet6)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:objects array - :array[6] :integer[1,6])

  (:action cmpswap
    :parameters ?i ?j - :integer[0,5]
    :precondition (< ?i ?j)
    :effect (:when (< array[?j] array[?i])
                   (:set array[?i] array[?j])
                   (:set array[?j] array[?i]))))


(define (problem p6)
  (:domain sortnet6)
  (:init (:set array[0] :in :integer[1,6])
         (:set array[1] :in :integer[1,6]
                            :assert (:not (= array[0] array[1])))
         (:set array[2] :in :integer[1,6]
                            :assert (:and (:not (= array[0] array[2]))
                                          (:not (= array[1] array[2]))))
         (:set array[3] :in :integer[1,6]
                            :assert (:and (:not (= array[0] array[3]))
                                          (:not (= array[1] array[3]))
                                          (:not (= array[2] array[3]))))
         (:set array[4] :in :integer[1,6]
                            :assert (:and (:not (= array[0] array[4]))
                                          (:not (= array[1] array[4]))
                                          (:not (= array[2] array[4]))
                                          (:not (= array[3] array[4]))))
         (:set array[5] :in :integer[1,6]
                            :assert (:and (:not (= array[0] array[5]))
                                          (:not (= array[1] array[5]))
                                          (:not (= array[2] array[5]))
                                          (:not (= array[3] array[5]))
                                          (:not (= array[4] array[5])))))
  (:goal (:and (< array[0] array[1])
               (< array[1] array[2])
               (< array[2] array[3])
               (< array[3] array[4])
               (< array[4] array[5]))))


;; forget about n=7!

(define (domain sortnet7)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:objects array - :array[7] :integer[1,7])

  (:action cmpswap
    :parameters ?i ?j - :integer[0,6]
    :precondition (< ?i ?j)
    :effect (:when (< array[?j] array[?i])
                   (:set array[?i] array[?j])
                   (:set array[?j] array[?i]))))


