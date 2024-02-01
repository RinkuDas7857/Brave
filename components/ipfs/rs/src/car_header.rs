// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

use crate::ffi::{
    CarV1Header, CarV1HeaderResult, CarV2Header, CarV2HeaderResult, Characteristics, ErrorData,
};
use cxx::CxxVector;
use libipld_cbor::DagCborCodec;
use libipld_core::{codec::Codec, ipld::Ipld};
use thiserror::Error;

const CARV1_VERSION: u64 = 1; 
const CARV2_VERSION: u64 = 2; 

//TODO move errors to separate module
#[derive(Error, Debug)]
enum DecodeCarHeaderError {
    #[error("Car header cbor codec error: `{reason}`")]
    HeaderCborDecodingError { reason: String },
    #[error("Car header expected cbor Map but got: `{reason}`")]
    HeaderCborMapExpectedError { reason: String },
    #[error("Car header, no roots found.")]
    HeaderRootsExpectedError,
    #[error("Car header version is unsupported: `{version}`")]
    HeaderVersionNotSupportedError { version: String },
    #[error("Internal error with code {0}")]
    InternalError(String, u16),
}
impl DecodeCarHeaderError {
    fn to_u16(&self) -> u16 {
        match self {
            DecodeCarHeaderError::HeaderCborDecodingError { .. } => 1u16,
            DecodeCarHeaderError::HeaderCborMapExpectedError { .. } => 10u16,
            DecodeCarHeaderError::HeaderRootsExpectedError => 20u16,
            DecodeCarHeaderError::HeaderVersionNotSupportedError { .. } => 30u16,
            DecodeCarHeaderError::InternalError(.., code) => *code,
        }
    }
}

/**
Takes header bytes (without block length prefix) and parse them as CAR_V1 header.
Returns error if failed.
|--------------- Header ---------------|
[ varint block length | CAR_V1 header  ]

*/
pub fn decode_carv1_header(data: &CxxVector<u8>) -> CarV1HeaderResult {
    match decode_carv_header_impl(data.as_slice(), CARV1_VERSION) {
        Ok((ver, roots)) => {
            CarV1HeaderResult {
                data: CarV1Header { version: ver, roots: roots },
                error: ErrorData { error: String::new(), error_code: 0 },
            }
        },
        Err(err) => {
            CarV1HeaderResult {
                data: CarV1Header { version: 0, roots: Vec::new() },
                error: ErrorData { error: err.error, error_code: err.error_code },
            }
        }
    }
}

/**
Takes header bytes (without block length prefix) and parse them as CAR_V1 header.
|--------------- Header -------------------|
[ pragma 11 bytes | CAR_V2 header 40 bytes ]
*/
pub fn decode_carv2_header(data: &CxxVector<u8>) -> CarV2HeaderResult {
    match decode_carv_header_impl(&data.as_slice()[1..11], CARV2_VERSION) {
        Ok((ver, _)) => ver,
        Err(err) => {
            println!("Error: {:?}", err);
            return CarV2HeaderResult {
                data: CarV2Header { characteristics: Characteristics { data: [0, 0] }, data_offset: 0, data_size: 0, index_offset: 0 },
                error: err,
            };
        }
    };

    let characteristics0 = u64::from_be_bytes(data.as_slice()[11..19].try_into().unwrap());
    let characteristics1 = u64::from_be_bytes(data.as_slice()[19..27].try_into().unwrap());

    let data_offset = u64::from_le_bytes(data.as_slice()[27..35].try_into().unwrap());
    let data_size = u64::from_le_bytes(data.as_slice()[35..43].try_into().unwrap());
    let index_offset = u64::from_le_bytes(data.as_slice()[43..51].try_into().unwrap());

    CarV2HeaderResult {
        data: CarV2Header {
            characteristics: Characteristics { data: [characteristics0, characteristics1] },
            data_offset: data_offset,
            data_size: data_size,
            index_offset: index_offset,
        },
        error: ErrorData { error: String::new(), error_code: 0 },
    }
}

fn decode_carv_header_impl(data: &[u8], expected_version: u64) -> Result<(u64, Vec<String>), ErrorData> {
    let header: Ipld = DagCborCodec
        .decode(data)
        .map_err(|e| {
            let error = DecodeCarHeaderError::HeaderCborDecodingError { reason: format!("{e:?}") };
            ErrorData { error: error.to_string(), error_code: error.to_u16() }
        })?;

    let header = if let Ipld::Map(map) = header {
        Ok(map)
    } else {
        let error =
            DecodeCarHeaderError::HeaderCborMapExpectedError { reason: format!("{:#?}", header) };
        Err(ErrorData { error: error.to_string(), error_code: error.to_u16() })
    }?;
    let roots = match header.get("roots") {
        Some(Ipld::List(roots_ipld)) => {
            let mut roots = Vec::with_capacity(roots_ipld.len());
            for root in roots_ipld {
                if let Ipld::Link(cid) = root {
                    roots.push(*cid);
                } else {
                    let error = DecodeCarHeaderError::InternalError(
                        format!(
                            "Car header, roots key elements expected cbor Link but got {:#?}",
                            root
                        ),
                        100u16,
                    );
                    return Err(ErrorData { error: error.to_string(), error_code: error.to_u16() });
                }
            }
            Ok(Some(roots))
        }
        Some(ipld) => {
            let error = DecodeCarHeaderError::InternalError(
                format!("Car header, roots key expected cbor List but got {:#?}", ipld),
                200u16,
            );
            Err(ErrorData { error: error.to_string(), error_code: error.to_u16() })
        }
        // CARv2 does not have 'roots' key, so allow to not be specified
        None => Ok(None),
    }?;

    let version = match header.get("version") {
        Some(Ipld::Integer(int)) => Ok(*int as u64),
        Some(ipld) => {
            let error = DecodeCarHeaderError::InternalError(
                format!("Car header, version key expected cbor Integer but got {:#?}", ipld),
                300u16,
            );
            Err(ErrorData { error: error.to_string(), error_code: error.to_u16() })
        }
        None => {
            let error = DecodeCarHeaderError::InternalError(
                format!(
                    "Car header, expected header key version, keys: {:?}",
                    header.keys().collect::<Vec<&String>>()
                ),
                400u16,
            );
            Err(ErrorData { error: error.to_string(), error_code: error.to_u16() })
        }
    }?;

    if version != expected_version {
        let error =
            DecodeCarHeaderError::HeaderVersionNotSupportedError { version: version.to_string() };
        return Err(ErrorData { error: error.to_string(), error_code: error.to_u16() });
    }

    let mut roots_str: Vec<String> = Vec::new();

    match roots {
        Some(r) => {
            for el in r {
                roots_str.push(el.to_string());
            }        
        },
        None => {
            if version == CARV1_VERSION {
                let error = DecodeCarHeaderError::HeaderRootsExpectedError {};
                return Err(ErrorData { error: error.to_string(), error_code: error.to_u16() });
            }
        }
    }

    Ok((version, roots_str))
}
