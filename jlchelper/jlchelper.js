// ==UserScript==
// @name         立创助手
// @namespace    http://tampermonkey.net/
// @version      1.23
// @description  None
// @author       Your Name
// @match        https://cart.szlcsc.com/cart/display.html*
// @match        https://www.szlcsc.com/huodong.html*
// @match        https://so.szlcsc.com/global.html*
// @grant        GM_setValue
// @grant        GM_getValue
// @grant        GM_registerMenuCommand
// @grant        GM_unregisterMenuCommand
// ==/UserScript==

(function() {
    'use strict';

    // 全局列表，用于存储菜单项的ID
    let menuCommandIds = [];

    // 获取当前页面的 URL
    const currentUrl = window.location.href;

    // 根据不同的页面 URL 执行不同的处理逻辑
    if (currentUrl.includes('cart.szlcsc.com/cart/display.html')) {
        handleCartPage();
        //observeDOMChanges(handleCartPage);
        setInterval(handleCartPage, 3000); // 每隔3秒重新生成一次悬浮表格
    } else if (currentUrl.includes('www.szlcsc.com/huodong.html')) {
        handleCouponPage();
    } else if (currentUrl.includes('so.szlcsc.com/global.html')) {
        handleSearchPage();
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

    function handleSearchPage() {
        console.log('处理搜索页面');
        // 预设的厂商列表
        var presetManufacturers = [];

        let saveManufacturersForSearch = GM_getValue('saveManufacturersForSearch', false);

         // 创建菜单项
        updateSearchMenu();

        if(saveManufacturersForSearch) {
            presetManufacturers = GM_getValue('presetManufacturers', presetManufacturers);
            if(presetManufacturers.length > 0) {
                console.log("saveManufacturersForSearch已启用，厂商读取：",presetManufacturers);
                // 定义点击确认按钮的函数
                function clickConfirmButton() {
                    var confirmButton = document.querySelector('input[type="button"][value="确定"]'); // 获取确认按钮元素
                    if (confirmButton) {
                        confirmButton.click(); // 点击确认按钮
                    }
                }

                // 提取多选栏中厂商名的函数
                function extractManufacturerName(label) {
                    var manufacturerName = label.getAttribute('title'); // 获取厂商名

                    // 如果厂商名包含括号，则提取括号内的内容作为厂商名
                    if (manufacturerName.includes('(') && manufacturerName.includes(')')) {
                        manufacturerName = manufacturerName.match(/\(([^)]+)\)/)[1];
                    }

                    return manufacturerName;
                }

                // 等待页面加载完成后执行选择操作和点击确认按钮
                window.addEventListener('load', function() {
                    var button = document.getElementById('more-brand'); // 获取按钮元素
                    if (button) {
                        button.click(); // 点击多选按钮

                        var checkboxes = document.querySelectorAll('input[type="checkbox"].fuxuanku'); // 获取所有复选框元素
                        var isAnypresetManufacturers = false;

                        checkboxes.forEach(function(checkbox) {
                            var label = checkbox.nextElementSibling; // 获取复选框相邻的label元素
                            var manufacturerName = extractManufacturerName(label);

                            if (presetManufacturers.includes(manufacturerName)) {
                                checkbox.checked = true; // 如果厂商名在预设列表中，则选中该复选框
                                isAnypresetManufacturers = true;
                            }
                        });
                        if (isAnypresetManufacturers) {
                            clickConfirmButton(); // 点击确认按钮
                        } else {
                            // 创建悬浮表格
                            const floatingTable = createFloatingTable();
                            const table = createTable();

                            // 添加表头
                            const headerRow = createRow(['总数', '未找到任何优惠厂商'], true);
                            table.appendChild(headerRow);
                            // 打印符合要求的厂商

                            // 将所有厂商名整理为一个 JSON 格式的字符串
                            const manufacturersJSON = JSON.stringify(presetManufacturers);

                            // 创建新行
                            const newRow = createRow([presetManufacturers.length, manufacturersJSON], false);

                            // 将新行添加到表格中
                            table.appendChild(newRow);

                            floatingTable.appendChild(table);
                            document.body.appendChild(floatingTable);
                        }
                    } else {
                        console.log("多选按钮不存在");
                    }
                });
            } else {
                console.log("saveManufacturersForSearch已启用，但厂商未读取：",presetManufacturers);
            }
        }
        /*
        // 获取所有的表格
        const tables = document.querySelectorAll('table');

        let foundAnyBrand = false;

        // 遍历每个表格
        tables.forEach(table => {
            // 获取当前表格内包含厂商名的元素
            const brandNameElements = table.querySelectorAll('.brand-name');

            // 遍历当前表格内的厂商名元素
            brandNameElements.forEach(element => {
                const brandName = element.innerText.trim();
                console.log("检测到" + brandName);

                // 判断提取出的厂商名是否在预置列表中
                if (!Brands.includes(brandName)) {
                    // 如果不在预置列表中，则移除该表格
                    table.remove();
                } else {
                    foundAnyBrand = true;
                }
            });
        });

        // 如果没有找到任何厂商名，随机延迟3-5秒后自动翻页
        if (!foundAnyBrand) {
            const randomWaitTime = Math.floor(Math.random() * 3000) + 3000; // 3-5秒
            setTimeout(() => {
                // 点击下一页按钮
                const nextPageButton = document.querySelector('.next');
                if (nextPageButton) {
                    nextPageButton.click();
                    // 设置定时任务，在2秒后再次执行handleSearchPage函数
                    setTimeout(() => {
                        handleSearchPage();
                    }, 2000);
                }
            }, randomWaitTime);
        }
        */
        // 更新菜单项 - Search Menu
        function updateSearchMenu() {
            // 移除现有菜单项
            menuCommandIds.forEach(id => GM_unregisterMenuCommand(id));
            menuCommandIds = [];

            menuCommandIds.push(createMenuCommand('领券页收集的优惠厂商列表用于多选搜索', saveManufacturersForSearch, '使用领券页收集的优惠厂商列表进行多选搜索？', 'saveManufacturersForSearch', true));
        }
    }



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

        // 更新菜单项 - Cart Menu
        function updateCartMenu() {
            // 移除现有菜单项
            menuCommandIds.forEach(id => GM_unregisterMenuCommand(id));
            menuCommandIds = [];

            // 添加 Cart Menu 的菜单项
            menuCommandIds.push(createMenuCommand('设置满多少', premiumBase, '请输入新的门槛:', 'premiumBase'));
            menuCommandIds.push(createMenuCommand('设置减多少', freeAmount, '请输入新的抵扣值:', 'freeAmount'));
            menuCommandIds.push(createMenuCommand('设置邮费', shippingFee, '请输入新的邮费:', 'shippingFee'));
        }
    }

    function handleCouponPage() {
        console.log('处理领券页面');

        // 获取用户设置的值
        let threshold = GM_getValue('threshold', 5);
        let disableNewUserCoupons = GM_getValue('disableNewUserCoupons', true);
        let deleteInvalidElements = GM_getValue('deleteInvalidElements', true);
        let saveManufacturersForSearch = GM_getValue('saveManufacturersForSearch', true);
        let browseOnly = GM_getValue('browseOnly', false);

        // 创建菜单项
        updateCouponMenu();
        // 用于存储满足要求的厂商名
        let validManufacturers = [];
        // 获取所有coupon-item元素
        const couponItems = document.querySelectorAll('.coupon-item');

        if (browseOnly) {
            // 仅浏览模式需要动态添加样式表
            const style = document.createElement('style');
            style.type = 'text/css';
            style.innerHTML = `
                .custom-button {
                    background-color: #8e44ad; /* button背景色 原抢券按钮为0094e7 */
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

            // 判断差值是否在阈值以内和是否置灰新人专享
            if ((Math.abs(money - condition) > threshold) || (disableNewUserCoupons && couponName.includes('<新人专享>'))) {
                if (deleteInvalidElements) {
                    item.remove(); // 删除不符合条件的元素
                } else {
                    disableElement(item);
                }
            } else {
                let couponNameElement = item.querySelector('.coupon-item.receive .coupon-item-name h3');
                if(couponNameElement){
                    couponNameElement = couponNameElement.textContent.trim();
                    //console.log('couponNameElement:', couponNameElement);
                    let manufacturer = couponNameElement.replace(/^\d+元/, '').replace(/品牌优惠$/, ''); // 裁剪提取厂商名
                    validManufacturers.push(manufacturer);
                }
            }


            // 处理仅浏览模式
            if (browseOnly) {
                const couponBtn = item.querySelector('.coupon-item-btn');
                if (couponBtn) {
                    const url = couponBtn.getAttribute('data-url');
                    couponBtn.remove(); // 移除原立即抢券按钮

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

        // 删除现有的浮动表格
        const existingFloatingTable = document.getElementById('floatingTable');
        if (existingFloatingTable) {
            existingFloatingTable.remove();
        }

        // 创建悬浮表格
        const floatingTable = createFloatingTable();
        const table = createTable();

        // 添加表头
        const headerRow = createRow(['总数', '可用优惠厂商'], true);
        table.appendChild(headerRow);
        // 打印符合要求的厂商

        // 将所有厂商名整理为一个 JSON 格式的字符串
        const manufacturersJSON = JSON.stringify(validManufacturers);

        // 创建新行
        const newRow = createRow([validManufacturers.length, manufacturersJSON], false);

        // 将新行添加到表格中
        table.appendChild(newRow);

        floatingTable.appendChild(table);
        document.body.appendChild(floatingTable);
        if(saveManufacturersForSearch) {
            GM_setValue('presetManufacturers', validManufacturers);
            //console.log("厂商已保存：",validManufacturers);
        } else {
            GM_setValue('presetManufacturers', []);
            //console.log("厂商未保存：",validManufacturers);
        }

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

        // 更新菜单项 - Coupon Menu
        function updateCouponMenu() {
            // 移除现有菜单项
            menuCommandIds.forEach(id => GM_unregisterMenuCommand(id));
            menuCommandIds = [];

            menuCommandIds.push(createMenuCommand('设置阈值', threshold, '请输入新的阈值:', 'threshold'));
            menuCommandIds.push(createMenuCommand('是否屏蔽新人专享券', disableNewUserCoupons, '新人专享券将被屏蔽？', 'disableNewUserCoupons', true));
            menuCommandIds.push(createMenuCommand('是否移除不符合条件的元素', deleteInvalidElements, '不符合条件的元素将被移除？', 'deleteInvalidElements', true));
            menuCommandIds.push(createMenuCommand('是否生成优惠厂商列表供搜索页多选', saveManufacturersForSearch, '生成优惠厂商列表供搜索页面多选？', 'saveManufacturersForSearch', true));
            menuCommandIds.push(createMenuCommand('仅浏览', browseOnly, '启用仅浏览模式？', 'browseOnly', true));
        }
    }

    // 创建菜单项
    function createMenuCommand(title, currentValue, promptMessage, valueKey, toggle = false) {
        return GM_registerMenuCommand(`${title} (当前: ${currentValue})`, () => {
            if (toggle) {
                currentValue = !currentValue;
            } else {
                const newValue = prompt(promptMessage, currentValue);
                if (newValue === null) {
                    return;
                }
                currentValue = parseInt(newValue, 10);
            }

            GM_setValue(valueKey, currentValue);
            const status = toggle ? (currentValue ? '启用' : '禁用') : currentValue;
            alert(`${title} 已设置为: ${status},将刷新页面使变更生效`);
            location.reload();
        });
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
        div.style.width = '450px';
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