// Copyright (c) 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

import * as React from 'react'

const SvgComponent = (props: any) => (
  <svg
    viewBox="0 0 288 61"
    fill="none"
    xmlns="http://www.w3.org/2000/svg"
    {...props}
  >
    <path
      fillRule="evenodd"
      clipRule="evenodd"
      d="M70.708 55.625H56.37a3.592 3.592 0 0 0-3.589 3.583c0 .991-.8 1.792-1.792 1.792-.99 0-1.793-.8-1.793-1.792a3.589 3.589 0 0 0-3.587-3.583H31.294c-.991 0-1.794-.8-1.794-1.792V19.792c0-.991.803-1.792 1.794-1.792h14.41l.013.002.013-.002c2.098 0 3.965.94 5.258 2.399 1.294-1.458 3.16-2.399 5.26-2.399.008 0 .015.005.024.005.01 0 .016-.005.025-.005h14.411c.99 0 1.792.8 1.792 1.792v34.041c0 .991-.803 1.792-1.792 1.792ZM49.5 25.415c0-1.882-1.543-3.413-3.442-3.413L46.046 22l-.013.002H33.5v30.016h12.533c.034 0 .063.018.096.02A7.072 7.072 0 0 1 49.5 53V25.415Zm19-3.411H55.988c-.009 0-.016-.004-.025-.004-.009 0-.016.004-.025.004-1.896 0-3.438 1.53-3.438 3.412V53a7.07 7.07 0 0 1 3.415-.97c.025-.001.046-.013.073-.013H68.5V22.003ZM14.301 39h-11.5C1.53 39 .5 38.104.5 37s1.03-2 2.301-2h11.5c1.27 0 2.3.896 2.3 2s-1.03 2-2.3 2ZM7.4 41H21.2c1.27 0 2.3.896 2.3 2s-1.03 2-2.3 2H7.399C6.13 45 5.1 44.104 5.1 43s1.03-2 2.3-2Zm2.3 6h6.9c1.272 0 2.3.896 2.3 2s-1.028 2-2.3 2H9.7c-1.271 0-2.3-.896-2.3-2s1.029-2 2.3-2Zm275.55.25h-18.008a4.51 4.51 0 0 0-4.507 4.5 2.247 2.247 0 0 1-2.25 2.25 2.25 2.25 0 0 1-2.252-2.25c0-2.48-2.018-4.5-4.505-4.5h-17.976A2.25 2.25 0 0 1 233.5 45V2.25A2.25 2.25 0 0 1 235.752 0h18.098l.016.002.015-.002c2.635 0 4.98 1.181 6.604 3.013C262.11 1.18 264.452 0 267.089 0c.012 0 .021.007.032.007s.02-.007.031-.007h18.098a2.25 2.25 0 0 1 2.25 2.25V45a2.25 2.25 0 0 1-2.25 2.25Zm-27.017-38.4a4.355 4.355 0 0 0-4.352-4.35l-.015-.002-.016.002h-15.848v38.25h15.848c.043 0 .079.022.121.025 1.555.04 3 .49 4.262 1.226V8.849Zm24.765-4.35h-15.846c-.011 0-.02-.004-.031-.004s-.02.004-.032.004c-2.4 0-4.354 1.95-4.354 4.35v35.153a8.915 8.915 0 0 1 4.325-1.235c.032-.002.059-.018.092-.018h15.846V4.5Zm-18.01 27a2.25 2.25 0 0 1 2.252-2.25h11.255a2.25 2.25 0 1 1 0 4.5H267.24a2.25 2.25 0 0 1-2.252-2.25Zm13.507-6.75H267.24a2.251 2.251 0 1 1 0-4.5h11.255a2.25 2.25 0 1 1 0 4.5Zm0-9H267.24a2.25 2.25 0 1 1 0-4.5h11.255a2.25 2.25 0 1 1 0 4.5Zm-36.019 18a2.25 2.25 0 1 1 0-4.5h11.255a2.25 2.25 0 1 1 0 4.5h-11.255Zm11.255-9h-11.255a2.25 2.25 0 1 1 0-4.5h11.255a2.25 2.25 0 1 1 0 4.5Zm0-9h-11.255a2.25 2.25 0 1 1 0-4.5h11.255a2.25 2.25 0 1 1 0 4.5ZM218.301 28h-11.5c-1.271 0-2.301-.896-2.301-2s1.03-2 2.301-2h11.5c1.27 0 2.299.896 2.299 2s-1.029 2-2.299 2Zm-6.901 2h13.801c1.269 0 2.299.896 2.299 2s-1.03 2-2.299 2H211.4c-1.27 0-2.3-.896-2.3-2s1.03-2 2.3-2Zm2.301 6h6.899c1.272 0 2.3.896 2.3 2s-1.028 2-2.3 2h-6.899c-1.272 0-2.301-.896-2.301-2s1.029-2 2.301-2Zm-43.393-24.984c2.803-.17 5.99 1.015 9.611 3.482 3.239-2.203 6.129-3.38 8.74-3.48l.458-.007.399.015c2.881.176 6.144 1.343 9.817 3.47.46.266.765.732.829 1.253l.013.197.149 25.323c.008 1.372-1.537 2.18-2.659 1.39-2.83-1.992-5.336-3.074-7.497-3.284-1.95-.19-4.941.826-8.864 3.124l-.294.177c-.261.228-.588.38-.95.401a1.578 1.578 0 0 1-1.307-.448c-4.041-2.399-7.11-3.458-9.097-3.264-2.161.21-4.667 1.292-7.497 3.284-1.068.752-2.521.055-2.65-1.198l-.009-.192.149-25.323a1.688 1.688 0 0 1 .842-1.45c3.672-2.127 6.935-3.294 9.817-3.47Zm18.732 3.37c-1.76-.019-3.972.796-6.598 2.489l-.497.327-.332.226-.014 10.349.014 10.694.256-.132c3.063-1.564 5.658-2.361 7.827-2.361l.428.01.372.028c1.757.17 3.6.746 5.533 1.715l.583.303.317.175-.125-21.264-.361-.194c-2.353-1.248-4.437-1.997-6.222-2.262l-.44-.056-.427-.036-.314-.012Zm-18.219-.01-.308.009c-1.86.113-4.048.793-6.549 2.054l-.583.302-.363.197-.125 21.261.319-.176c1.734-.93 3.397-1.552 4.992-1.855l.594-.099.53-.064c2.186-.213 4.844.473 8.025 2.023l.602.3.254.133.015-10.68-.015-10.362-.275-.19c-2.686-1.816-4.957-2.748-6.777-2.845l-.336-.009Zm-68.513 13.64c2.803-.17 5.99 1.015 9.611 3.482 3.239-2.203 6.129-3.38 8.74-3.48l.458-.006.399.014c2.881.176 6.144 1.343 9.817 3.47.46.267.765.732.829 1.253l.013.197.149 25.323c.008 1.372-1.537 2.18-2.659 1.39-2.83-1.992-5.336-3.074-7.497-3.284-1.95-.19-4.941.826-8.864 3.124l-.294.177c-.261.228-.588.38-.95.401a1.578 1.578 0 0 1-1.307-.448c-4.041-2.399-7.11-3.458-9.097-3.264-2.161.21-4.667 1.292-7.497 3.284-1.068.752-2.52.055-2.65-1.198l-.009-.192.15-25.323a1.687 1.687 0 0 1 .84-1.45c3.673-2.127 6.936-3.294 9.818-3.47Zm18.732 3.37c-1.76-.019-3.972.796-6.598 2.489l-.497.328-.332.225-.014 10.349.014 10.694.256-.132c3.063-1.564 5.658-2.361 7.827-2.361l.428.01.372.028c1.757.17 3.6.746 5.533 1.715l.583.303.317.175-.125-21.264-.361-.194c-2.353-1.248-4.437-1.997-6.222-2.262l-.44-.056-.427-.036-.314-.012Zm-18.219-.01-.308.009c-1.86.113-4.048.793-6.55 2.054l-.582.302-.363.197-.125 21.261.319-.176c1.734-.93 3.397-1.552 4.992-1.855l.594-.099.53-.064c2.186-.213 4.844.473 8.025 2.023l.602.3.254.133.015-10.68-.015-10.362-.275-.19c-2.686-1.816-4.957-2.748-6.777-2.845l-.336-.009Z"
      fill="currentColor"
    />
  </svg>
)

export default SvgComponent