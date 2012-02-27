/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    GaussNewtonOptimizer.h
 * @brief   
 * @author  Richard Roberts
 * @created Feb 26, 2012
 */

#pragma once

#include <gtsam/nonlinear/NonlinearOptimizer.h>

namespace gtsam {

/** Parameters for Gauss-Newton optimization, inherits from
 * NonlinearOptimizationParams.
 */
class GaussNewtonParams : public NonlinearOptimizerParams {
public:
  /** See GaussNewtonParams::elimination */
  enum Elimination {
    MULTIFRONTAL,
    SEQUENTIAL
  };

  /** See GaussNewtonParams::factorization */
  enum Factorization {
    LDL,
    QR,
  };

  Elimination elimination; ///< The elimination algorithm to use (default: MULTIFRONTAL)
  Factorization factorization; ///< The numerical factorization (default: LDL)
  Ordering ordering; ///< The variable elimination ordering (default: empty -> COLAMD)

  GaussNewtonParams() :
    elimination(MULTIFRONTAL), factorization(LDL) {}

  virtual void print(const std::string& str = "") const {
    NonlinearOptimizerParams::print(str);
    if(elimination == MULTIFRONTAL)
      std::cout << "         elimination method: MULTIFRONTAL\n";
    else if(elimination == SEQUENTIAL)
      std::cout << "         elimination method: SEQUENTIAL\n";
    else
      std::cout << "         elimination method: (invalid)\n";

    if(factorization == LDL)
      std::cout << "       factorization method: LDL\n";
    else if(factorization == QR)
      std::cout << "       factorization method: QR\n";
    else if(factorization == CHOLESKY)
      std::cout << "       factorization method: CHOLESKY\n";
    else
      std::cout << "       factorization method: (invalid)\n";

    std::cout.flush();
  }
};

/**
 * This class performs Gauss-Newton nonlinear optimization
 */
class GaussNewtonOptimizer : public NonlinearOptimizer {

public:

  typedef boost::shared_ptr<GaussNewtonParams> SharedGNParams;

  /// @name Standard interface
  /// @{

  /** Standard constructor, requires a nonlinear factor graph, initial
   * variable assignments, and optimization parameters.  For convenience this
   * version takes plain objects instead of shared pointers, but internally
   * copies the objects.
   * @param graph The nonlinear factor graph to optimize
   * @param values The initial variable assignments
   * @param params The optimization parameters
   */
  GaussNewtonOptimizer(const NonlinearFactorGraph& graph, const Values& values,
      const GaussNewtonParams& params) :
        NonlinearOptimizer(
            SharedGraph(new NonlinearFactorGraph(graph)),
            SharedValues(new Values(values)),
            SharedGNParams(new GaussNewtonParams(params))),
        gnParams_(boost::static_pointer_cast<GaussNewtonParams>(params_)) {}

  /** Standard constructor, requires a nonlinear factor graph, initial
   * variable assignments, and optimization parameters.
   * @param graph The nonlinear factor graph to optimize
   * @param values The initial variable assignments
   * @param params The optimization parameters
   */
  GaussNewtonOptimizer(const SharedGraph& graph, const SharedValues& values,
      const SharedGNParams& params) :
        NonlinearOptimizer(graph, values, params), gnParams_(params) {}

  /// @}

  /// @name Advanced interface
  /// @{

  /** Virtual destructor */
  virtual ~GaussNewtonOptimizer() {}

  /** Perform a single iteration, returning a new NonlinearOptimizer class
   * containing the updated variable assignments, which may be retrieved with
   * values().
   */
  virtual auto_ptr iterate() const;

  /** Update the graph, values, and/or parameters, leaving all other state
   * the same.  Any of these that are empty shared pointers are left unchanged
   * in the returned optimizer object.  Returns a new updated
   * NonlinearOptimzier object, the original is not modified.
   */
  virtual auto_ptr update(
      const SharedGraph& newGraph = SharedGraph(),
      const SharedValues& newValues = SharedValues(),
      const SharedParams& newParams = SharedParams()) const {
    return new GaussNewtonOptimizer(*this, newGraph, newValues,
        boost::dynamic_pointer_cast<GaussNewtonParams>(newParams));
  }

  /** Create a copy of the NonlinearOptimizer */
  virtual auto_ptr clone() const {
    return new GaussNewtonOptimizer(*this);
  }

  /// @}

protected:

  const SharedGNParams gnParams_;

  GaussNewtonOptimizer(const SharedGraph& graph, const SharedValues& values,
      const SharedGNParams& params, double error, int iterations) :
    NonlinearOptimizer(graph, values, params, error, iterations), gnParams_(params) {}

  GaussNewtonOptimizer(const GaussNewtonOptimizer& original, const SharedGraph& newGraph,
      const SharedValues& newValues, const SharedGNParams& newParams) :
    NonlinearOptimizer(original, newGraph, newValues, newParams),
    gnParams_(newParams ? newParams : original.gnParams_) {}

};

}
