//
// messages
//

// fatal messages
FATALMSG(usage,1000,0,"usage: %s <problem-name> <pddl-file>+\n")

// error messages
ERRORMSG(nameClash,2000,1,"name clash with \"%s\"")
ERRORMSG(unsupportedFeature,2001,0,"unsupported feature")
ERRORMSG(undefinedModel,2002,0,"undefined model")
ERRORMSG(undefinedDomain,2003,1,"undefined domain \"%s\"")
ERRORMSG(undefinedFunction,2004,1,"undefined function \"%s\"")
ERRORMSG(undefinedPredicate,2005,1,"undefined predicate \"%s\"")
ERRORMSG(undefinedVariable,2006,1,"undefined variable \"%s\"")
ERRORMSG(undefinedObject,2007,1,"undefined object \"%s\"")
ERRORMSG(undefinedType,2008,1,"undefined type \"%s\"")
ERRORMSG(incompTypeEquality,2009,0,"incompatible types in equality")
ERRORMSG(incompTypePredicate,2010,1,"incompatible types in predicate \"%s\"")
ERRORMSG(incompTypeFunction,2011,1,"incompatible types in function \"%s\"")
ERRORMSG(incompTypeAssignment,2012,0,"incompatible types in assignment")
ERRORMSG(probabilitySum,2013,0,"probabilities must sum to 1.0")
ERRORMSG(wrongPackageName,2014,2,"wrong package name \"%s\" (expecting \"%s\")")
ERRORMSG(invalidSubscript,2015,1,"invalid subscript for \"%s\"")
ERRORMSG(invalidLValue,2016,0,"invalid l-value in assignment")
ERRORMSG(invalidRValue,2017,0,"invalid r-value in assignment")
ERRORMSG(invalidInternalFunction,2018,1,"invalid internal function \"%s\"")
ERRORMSG(badRangeLimits,2019,2,"bad range limits: [%s,%s]")
ERRORMSG(featureInModel,2020,0,"")
ERRORMSG(undefinedFunPred,2021,1,"undefined function/predicate \"%s\"")
ERRORMSG(invalidInternalPredicate,2022,1,"invalid internal predicate \"%s\"")
ERRORMSG(incompTypeInSum,2023,0,"invalid term in sum expression")
ERRORMSG(incompTypeInConditional,2024,0,"types in conditional term are not of same type")
ERRORMSG(badCostTerm,2025,0,"invalid cost term")
ERRORMSG(badStarTerm,2026,0,"invalid star term")

// warnings
WARNINGMSG(featureInModelWarning,3000,1,"\"%s\" has no effect in this problem class.")
WARNINGMSG(probabilitySumWarning,3001,1,"probabilities add to %f. Assuming default effect.")
WARNINGMSG(unableCheckInteger,3002,0,"unable to check integer compatibility.")
