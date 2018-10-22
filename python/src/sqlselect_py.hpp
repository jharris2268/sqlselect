#ifndef SQLSELECT_PY_HPP
#define SQLSELECT_PY_HPP

#include "sqlselect.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "picojson.h"

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(XX, std::shared_ptr<XX>);

PYBIND11_MAKE_OPAQUE(sqlselect::KeyValueMap);
PYBIND11_MAKE_OPAQUE(sqlselect::Feature_vec);
PYBIND11_MAKE_OPAQUE(sqlselect::Data_map);

void export_geojson(py::module& m);
void export_mvt(py::module& m);

#endif 
