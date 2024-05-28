// ==UserScript==
// @name         立创助手
// @namespace    http://tampermonkey.net/
// @version      1.18
// @description  None
// @author       Your Name
// @match        https://cart.szlcsc.com/cart/display.html*
// @match        https://www.szlcsc.com/huodong.html*
// @grant        GM_setValue
// @grant        GM_getValue
// @grant        GM_registerMenuCommand
// @grant        GM_unregisterMenuCommand
// ==/UserScript==

(function() {
    'use strict';

    // 全局列表，用于存储菜单项的ID
    let cartMenuCommandIds = [];
    let couponMenuCommandIds = [];

    // 获取当前页面的 URL
    const currentUrl = window.location.href;

    // 根据不同的页面 URL 执行不同的处理逻辑
    if (currentUrl.includes('cart.szlcsc.com/cart/display.html')) {
        handleCartPage();
        //observeDOMChanges(handleCartPage);
        setInterval(handleCartPage, 3000); // 每隔3秒重新生成一次悬浮表格
    } else if (currentUrl.includes('www.szlcsc.com/huodong.html')) {
        handleCouponPage();
    }

/*     function debounce(func, wait) {
        let timeout;
        return function(...args) {
            clearTimeout(timeout);
            timeout = setTimeout(() => func.apply(this, args), wait);
        };
    }


    function observeDOMChanges(callback) {
        const targetNodes = document.querySelectorAll('.product-item');
        const config = { childList: true, subtree: true };

        const debouncedCallback = debounce(callback, 500); // 500ms 防抖

        const observer = new MutationObserver((mutationsList, observer) => {
            for (const mutation of mutationsList) {
                if (mutation.type === 'childList') {
                // if (mutation.type === 'childList' && (mutation.addedNodes.length > 0 || mutation.removedNodes.length > 0)) {
                    debouncedCallback();
                    break;
                }
            }
        });

        targetNodes.forEach(node => observer.observe(node, config));
    } */

    function handleCartPage() {
        console.log('处理购物车页面');

        // 获取用户设置的值
        let premiumBase = GM_getValue('premiumBase', 16);
        let freeAmount = GM_getValue('freeAmount', 15);
        let shippingFee = GM_getValue('shippingFee', 15);

        // 创建菜单项
        updateCartMenu();

        // 获取所有商品行
        const productItems = document.querySelectorAll('.product-item');

        // 存储厂商和总价的映射
        const vendorTotalPriceMap = {};
        const vendorWarehouseMap = {};
        const vendorProductMap = {};
        let overallTotalPrice = 0;
        let overallPremium = 0;

        productItems.forEach(item => {
            const productCode = item.querySelector('a').innerText;
            //const vendor = item.querySelector('.cart-li-pro-info .ellipsis:last-child').innerText.trim();
            const vendor = item.querySelector('.cart-li-pro-info .ellipsis:nth-child(3)').innerText.trim();
            const warehouseElement = item.querySelector('.warehouse');
            const warehouse = warehouseElement ? warehouseElement.childNodes[0].nodeValue.trim() : '未知仓库';
            const totalPriceText = item.querySelector('.line-total-price').innerText;
            const totalPrice = parseFloat(totalPriceText.replace('￥', ''));

            // 累加每个厂商的总价
            vendorTotalPriceMap[vendor] = (vendorTotalPriceMap[vendor] || 0) + totalPrice;

            // 记录每个厂商的仓库信息
            if (!vendorWarehouseMap[vendor]) {
                vendorWarehouseMap[vendor] = {};
            }
            vendorWarehouseMap[vendor][warehouse] = (vendorWarehouseMap[vendor][warehouse] || 0) + 1;

            // 记录每个厂商的产品编号
            if (!vendorProductMap[vendor]) {
                vendorProductMap[vendor] = [];
            }
            vendorProductMap[vendor].push(productCode);

            // 累加所有厂商的总价
            overallTotalPrice += totalPrice;
        });

         // 删除现有的浮动表格
        const existingFloatingTable = document.getElementById('floatingTable');
        if (existingFloatingTable) {
            existingFloatingTable.remove();
        }

        // 创建悬浮表格
        const floatingTable = createFloatingTable();
        const table = createTable();

        // 添加表头
        const headerRow = createRow(['序号', '厂商', '总价', '溢价'], true);
        table.appendChild(headerRow);

        let originalOverallPremium = 0;
        let index = 1;
        // 添加每个厂商的信息行
        for (const [vendor, totalPrice] of Object.entries(vendorTotalPriceMap)) {
            const vendorText = `${vendor} (${Object.entries(vendorWarehouseMap[vendor]).map(([warehouse, count]) => `${warehouse}: ${count}`).join(', ')})`;
            const premium = totalPrice - premiumBase;
            // 计算显示的溢价：如果不足premiumBase，则显示实际premium(此时应该为负数)。如果已经满足premiumBase，则还需要加上(premiumBase - freeAmount)
            const displayPremium = premium < 0 ? premium : premium + (premiumBase - freeAmount);
            // 计算总溢价：如果不足premiumBase，则使用totalPrice。如果已经满足premiumBase，则还需要加上(premiumBase - freeAmount)
            originalOverallPremium += premium < 0 ? totalPrice : premium + (premiumBase - freeAmount);
            const row = createRow([index, vendorText, `￥${totalPrice.toFixed(2)}`, `￥${displayPremium.toFixed(2)}`]);

            // 添加点击事件
            row.cells[1].innerHTML = `<a href="javascript:void(0)">${vendorText}</a>`;
            row.cells[1].addEventListener('click', () => {
                row.cells[1].innerText = `${vendorText} - 编号: ${vendorProductMap[vendor].join(', ')}`;
                row.cells[1].style.cursor = 'default';
            });

            table.appendChild(row);
            index++;
        }

        // 添加总计行
        const totalRow = createRow(['总价', `￥${overallTotalPrice.toFixed(2)}`, ''], false, 2);
        totalRow.cells[0].colSpan = 2;
        totalRow.cells[1].colSpan = 2;
        table.appendChild(totalRow);

        // 添加总溢价行
        const totalPremiumRow = createRow(['总溢价', `￥${originalOverallPremium.toFixed(2)} + 邮费￥${shippingFee.toFixed(2)} = ￥${(originalOverallPremium + shippingFee).toFixed(2)}`, ''], false, 2);
        totalPremiumRow.cells[0].colSpan = 2;
        totalPremiumRow.cells[1].colSpan = 2;
        table.appendChild(totalPremiumRow);

        floatingTable.appendChild(table);
        document.body.appendChild(floatingTable);

        // 更新菜单项
        function updateCartMenu() {
            // 移除现有菜单项
            cartMenuCommandIds.forEach(id => GM_unregisterMenuCommand(id));
            cartMenuCommandIds = [];

            cartMenuCommandIds.push(GM_registerMenuCommand(`设置满多少 (当前: ${premiumBase})`, () => {
                const newPremiumBase = prompt('请输入新的门槛:', premiumBase);
                if (newPremiumBase !== null) {
                    premiumBase = parseInt(newPremiumBase, 10);
                    GM_setValue('premiumBase', premiumBase);
                    alert(`门槛已设置为: ${premiumBase},请刷新页面使变更生效`);
                    updateCartMenu();
                }
            }));

            cartMenuCommandIds.push(GM_registerMenuCommand(`设置减多少 (当前: ${freeAmount})`, () => {
                const newfreeAmount = prompt('请输入新的抵扣值:', freeAmount);
                if (newfreeAmount !== null) {
                    freeAmount = parseInt(newfreeAmount, 10);
                    GM_setValue('freeAmount', freeAmount);
                    alert(`抵扣值已设置为: ${freeAmount},请刷新页面使变更生效`);
                    updateCartMenu();
                }
            }));

            cartMenuCommandIds.push(GM_registerMenuCommand(`设置邮费 (当前: ${shippingFee})`, () => {
                const newShippingFee = prompt('请输入新的邮费:', shippingFee);
                if (newShippingFee !== null) {
                    shippingFee = parseInt(newShippingFee, 10);
                    GM_setValue('shippingFee', shippingFee);
                    alert(`邮费已设置为: ${shippingFee},请刷新页面使变更生效`);
                    updateCartMenu();
                }
            }));
        }
    }

    function handleCouponPage() {
        console.log('处理领券页面');

        // 获取用户设置的值
        let threshold = GM_getValue('threshold', 5);
        let disableNewUserCoupons = GM_getValue('disableNewUserCoupons', true);
        let deleteInvalidElements = GM_getValue('deleteInvalidElements', true);
        let browseOnly = GM_getValue('browseOnly', false);

        // 创建菜单项
        updateCouponMenu();

        // 获取所有coupon-item元素
        const couponItems = document.querySelectorAll('.coupon-item');

        if (browseOnly) {
            // 仅浏览模式需要动态添加样式表
            const style = document.createElement('style');
            style.type = 'text/css';
            style.innerHTML = `
                .custom-button {
                    background-color: #0094e7; /* 背景色 */
                    border: none; /* 无边框 */
                    color: white; /* 白色文字 */
                    padding: 4px 72px; /* 内边距 */
                    text-align: center; /* 文字居中 */
                    text-decoration: none; /* 无下划线 */
                    display: inline-block; /* 内联块 */
                    font-size: 14px; /* 字体大小 */
                    margin: 10px 4px 0px 4px; /* 外边距 */
                    cursor: pointer; /* 鼠标指针 */
                    border-radius: 5px; /* 圆角 */
                }
            `;
            document.head.appendChild(style);
        }

        // 遍历每个coupon-item元素
        couponItems.forEach(item => {
            let conditionElement = item.querySelector('.condition');
            if (!conditionElement) {
                //console.log('已使用的券 item:', item);
                conditionElement = item.querySelector('.condition-disable');
            }
            if (!conditionElement) {
                console.log('Error: condition element not found for item:', item);
                return; // 如果仍找不到.condition元素，直接跳过这个item并报错
            }

            const condition = parseInt(conditionElement.textContent.trim().replace('满', '').replace('可用', ''));
            const moneyElement = item.querySelector('.money');
            let money = condition; // 默认设置为condition，以防止空指针异常
            if (moneyElement) {
                const moneyText = moneyElement.textContent.trim().match(/\d+/);
                if (moneyText) {
                    money = parseInt(moneyText[0]);
                }
            }
            const couponName = item.querySelector('.coupon-item-name h3').textContent.trim();

            // 判断差值是否在阈值以内
            if (Math.abs(money - condition) > threshold) {
                if (deleteInvalidElements) {
                    item.remove(); // 删除不符合条件的元素
                } else {
                    disableElement(item);
                }
            }

            // 判断是否置灰新人专享
            if (disableNewUserCoupons && couponName.includes('<新人专享>')) {
                if (deleteInvalidElements) {
                    item.remove(); // 删除不符合条件的元素
                } else {
                    disableElement(item);
                }
            }

            // 处理仅浏览模式
            if (browseOnly) {
                const couponBtn = item.querySelector('.coupon-item-btn');
                if (couponBtn) {
                    const url = couponBtn.getAttribute('data-url');
                    couponBtn.remove(); // 移除立即抢券部分

                    const newBtn = document.createElement('button');
                    newBtn.innerText = '浏览';
                    newBtn.className = 'custom-button'; // 应用自定义样式类
                    newBtn.addEventListener('click', () => {
                        window.open(url, '_blank');
                    });
                    item.querySelector('.coupon-item-con').appendChild(newBtn);
                }
            }
        });

        // 禁用元素的函数
        function disableElement(item) {
            const btnText = item.querySelector('.coupon-item-btn-text');
            const btn = item.querySelector('.coupon-item-btn');
            btnText.style.color = 'gray';
            btnText.textContent = '不可点击'; // 修改为不可点击状态
            btn.style.pointerEvents = 'none'; // 禁用点击
            btn.style.background = 'gray';
            btn.style.color = 'gray';
        }

        // 更新菜单项
        function updateCouponMenu() {
            // 移除现有菜单项
            couponMenuCommandIds.forEach(id => GM_unregisterMenuCommand(id));
            couponMenuCommandIds = [];

            couponMenuCommandIds.push(GM_registerMenuCommand(`设置阈值 (当前: ${threshold})`, () => {
                const newThreshold = prompt('请输入新的阈值:', threshold);
                if (newThreshold !== null) {
                    threshold = parseInt(newThreshold, 10);
                    GM_setValue('threshold', threshold);
                    alert(`阈值已设置为: ${threshold},请刷新页面使变更生效`);
                    updateCouponMenu();
                }
            }));

            couponMenuCommandIds.push(GM_registerMenuCommand(`是否屏蔽新人专享券 (当前: ${disableNewUserCoupons ? '启用' : '禁用'})`, () => {
                disableNewUserCoupons = !disableNewUserCoupons;
                GM_setValue('disableNewUserCoupons', disableNewUserCoupons);
                alert(`新人专享券将${disableNewUserCoupons ? '' : '不再'}被屏蔽,请刷新页面使变更生效`);
                updateCouponMenu();
            }));

            couponMenuCommandIds.push(GM_registerMenuCommand(`是否移除不符合条件的元素 (当前: ${deleteInvalidElements ? '启用' : '禁用'})`, () => {
                deleteInvalidElements = !deleteInvalidElements;
                GM_setValue('deleteInvalidElements', deleteInvalidElements);
                alert(`不符合条件的元素将${deleteInvalidElements ? '' : '不再'}被移除,请刷新页面使变更生效`);
                updateCouponMenu();
            }));

            couponMenuCommandIds.push(GM_registerMenuCommand(`仅浏览 (当前: ${browseOnly ? '启用' : '禁用'})`, () => {
                browseOnly = !browseOnly;
                GM_setValue('browseOnly', browseOnly);
                alert(`仅浏览模式将${browseOnly ? '启用' : '禁用'},请刷新页面使变更生效`);
                updateCouponMenu();
            }));
        }
    }

    // 创建悬浮表格容器
    function createFloatingTable() {
        const div = document.createElement('div');
        div.id = 'floatingTable'; // 添加一个唯一的ID以便识别和删除
        div.style.position = 'fixed';
        div.style.top = '50px';
        div.style.right = '10px';
        div.style.backgroundColor = 'white';
        div.style.border = '1px solid black';
        div.style.padding = '10px';
        div.style.zIndex = '1000';
        return div;
    }

    // 创建表格
    function createTable() {
        const table = document.createElement('table');
        table.style.borderCollapse = 'collapse';
        table.style.width = '100%';
        return table;
    }

    // 创建表格行
    function createRow(cellValues, isHeader = false, colSpan = 1) {
        const row = document.createElement('tr');
        cellValues.forEach((value, index) => {
            const cell = isHeader ? document.createElement('th') : document.createElement('td');
            cell.innerText = value;
            cell.style.border = '1px solid black';
            if (index === 1 && colSpan > 1) {
                cell.colSpan = colSpan;
            }
            row.appendChild(cell);
        });
        return row;
    }
})();
