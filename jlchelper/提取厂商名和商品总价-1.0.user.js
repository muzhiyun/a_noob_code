// ==UserScript==
// @name         提取厂商名和商品总价
// @namespace    https://cart.szlcsc.com/cart/display.html
// @version      1.0
// @description  提取每行中的特定格式厂商名称和商品总价，计算每个厂商的总价，并在页面上创建一个浮动表格显示每个商家及其名下所有商品的总价之和
// @author       Your Name
// @match        https://cart.szlcsc.com/cart/display.html
// @grant        none
// ==/UserScript==

(function() {
    'use strict';

    let totalPrices = {};

    const productItems = document.querySelectorAll('.product-item');

    productItems.forEach((productItem) => {
        const manufacturerElement = productItem.querySelector('.ellipsis');
        const manufacturerText = manufacturerElement.innerText.trim();
        const manufacturer = manufacturerText.replace(/\s+/g, ' '); // 去除多余空格
        console.debug("厂商"+manufacturerText);
        const price = parseFloat(productItem.querySelector('.line-total-price').innerText.replace('￥', ''));

        if (totalPrices[manufacturer]) {
            totalPrices[manufacturer] += price;
        } else {
            totalPrices[manufacturer] = price;
        }
    });

    const floatingTable = document.createElement('div');
    floatingTable.style.position = 'fixed';
    floatingTable.style.top = '50px';
    floatingTable.style.right = '10px';
    floatingTable.style.background = '#fff';
    floatingTable.style.padding = '10px';
    floatingTable.style.border = '1px solid #ccc';

    for (const [manufacturer, totalPrice] of Object.entries(totalPrices)) {
        const row = document.createElement('div');
        row.textContent = `${manufacturer}: ￥${totalPrice.toFixed(2)}`;
        floatingTable.appendChild(row);
    }

    document.body.appendChild(floatingTable);
})();
